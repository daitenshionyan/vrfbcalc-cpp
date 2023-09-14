#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>

#include "strutils.hpp"
#include "vrfbcalccfg.hpp"


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
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete ui;
}


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
