#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>
#include <QFileDialog>

#include "strutils.hpp"
#include "vrfbcalccfg.hpp"
#include "driver/vrfbdriver_io.hpp"


namespace {


class PromLogger : public logger::Logger {
  public:
    PromLogger(QPromise<logger::LogMsg>& prom) : prom_p{&prom} {}

    void log(const logger::LogMsg& lm) {
      prom_p->addResult(lm);
    }


  private:
    QPromise<logger::LogMsg>* prom_p;
};


}


MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        popup_ce(new CEConfigPopup(this)) {
  ui->setupUi(this);
  ui->outputArea->appendHtml(
      QString("<p style=\"color:grey;white-space:pre\">")
      + QString::fromStdString(
            strutils::format_string(
                "%s\n%s\n%s",
                "================================================================================",
                vrfbcfg::licence_notice,
                "================================================================================"))
      + QString("</p>"));
  connect(this, &MainWindow::availableLogMsg,
      this, &MainWindow::logMsg);
  connect(popup_ce, &CEConfigPopup::accepted,
      this, &MainWindow::startCalc);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::resultReadyAt,
      this, &MainWindow::logMsgAt);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::started,
      this, &MainWindow::disableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::canceled,
      this, &MainWindow::enableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::finished,
      this, &MainWindow::enableActions);
  connect(this, &MainWindow::completedPerformanceReading,
      this, &MainWindow::displayPerformanceView,
      Qt::ConnectionType::QueuedConnection);
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete ui;
}


void MainWindow::startCalc() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<logger::LogMsg>& p) {
        auto w = PromLogger{p};
        vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), w);
      }));
}


void MainWindow::log(const logger::LogMsg& lm) {
  emit availableLogMsg(lm);
}


void MainWindow::logMsg(const logger::LogMsg& lm) {
  std::string color = "black";
  std::string lvlTxt = "";
  switch (lm.lvl) {
    case logger::Level::kFine:
      color = "#A0A0A0";
      lvlTxt = "FINE";
      break;
    case logger::Level::kInfo:
      color = "white";
      lvlTxt = "INFO";
      break;
    case logger::Level::kWarn:
      color = "#FF9933";
      lvlTxt = "WARN";
      break;
    case logger::Level::kFail:
      color = "#FF3333";
      lvlTxt = "FAIL";
      break;
    case logger::Level::kSucc:
      color = "#33FF33";
      lvlTxt = "SUCC";
  }
  QString outText = QString::fromStdString(
      strutils::format_string("[%s] [%s] %s",
          strutils::getftime().c_str(),
          lvlTxt.c_str(),
          lm.msg.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml(
      QString::fromStdString(
          strutils::format_string("<p style=\"color:%s;white-space:pre\">",
              color.c_str()))
      + outText + "</p>");
}


void MainWindow::disableActions() {
  ui->menuRun->setDisabled(true);
}


void MainWindow::enableActions() {
  ui->menuRun->setDisabled(false);
}


void MainWindow::displayPerformanceView(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  CEResultView* rv = new CEResultView(this);
  connect(rv, &CEResultView::exportRequested,
      this, &MainWindow::exportCEPerformance);
  rv->plotGraphs(entries);
  rv->open();
}


void MainWindow::exportCEPerformance(CEResultView* rv) {
  if (watcher.isRunning()) {
    warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        bool is_success = rv->exportImages(l);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}


// ---- < SLOTS > --------------------------------------------------------------


void MainWindow::on_action_openOutput_triggered(bool) {
  std::filesystem::path output_path {"output"};
  if (!std::filesystem::exists(output_path)) {
    fail("Output folder does not exist yet");
    return;
  } else if (!std::filesystem::is_directory(output_path)) {
    fail("The file 'output' exists but it is not a directory");
    return;
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile("output"));
}


void MainWindow::on_actionCECalculations_triggered(bool) {
  if (watcher.isRunning()) {
    warn("Cannot perform calculations as another process is already running");
    return;
  }
  popup_ce->exec();
}


void MainWindow::on_actionCEAnalysis_triggered(bool) {
  if (watcher.isRunning()) {
    warn("Cannot analyse data as another process is already running");
    return;
  }
  QString openPath = "output";
  if (!std::filesystem::exists(openPath.toStdString())) {
    openPath = QString();
  }
  QStringList qstrPaths = QFileDialog::getOpenFileNames(
      this,
      "Select one or more performance files to open",
      openPath,
      "XLSX Files (*.xlsx)"
  );
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, qstrPaths](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        std::vector<std::string> strPaths {};
        for (const QString& qstrPath : qstrPaths) {
          strPaths.push_back(qstrPath.toStdString());
        }
        emit completedPerformanceReading(vrfbdriver::readPerformance_CE(strPaths, l));
      }));
}
