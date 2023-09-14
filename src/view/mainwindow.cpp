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


std::vector<vrfb::Table> readPerformanceData(const QStringList& strPaths, logger::Logger& l) {
  std::vector<vrfb::Table> tables {};
  for (const QString& strPath : strPaths) {
    auto path = std::filesystem::u8path<std::string>(strPath.toStdString());
    try {
      tables.push_back(vrfbdriver::io::readTable_XLSX(path, "Dat"));
    } catch (std::exception& ex) {
      l.fail(strutils::format_string("Failed to read '%s' - %s",
          path.string().c_str(), ex.what()));
    }
  }
  l.fine(strutils::format_string("Successfully read %d performance data",
      tables.size()));
  return tables;
}


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


void MainWindow::displayPerformanceView(const std::vector<vrfb::Table>& tables) {
  CEResultView* rv = new CEResultView(this);
  rv->createGraphs(tables);
  rv->show();
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


void MainWindow::on_startBtn_clicked() {
  if (watcher.isRunning()) {
    return;
  }
  popup_ce->exec();
}


void MainWindow::on_actionPerformanceAnalysis_triggered(bool) {
  if (watcher.isRunning()) {
    warn("Cannot analyse data as another process is already running");
    return;
  }
  QString openPath = "output";
  if (!std::filesystem::exists(openPath.toStdString())) {
    openPath = QString();
  }
  QStringList files = QFileDialog::getOpenFileNames(
      this,
      "Select one or more performance files to open",
      openPath,
      "XLSX Files (*.xlsx)"
  );
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, files](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        std::vector<vrfb::Table> tables = readPerformanceData(files, l);
        emit completedPerformanceReading(tables);
      }));
}
