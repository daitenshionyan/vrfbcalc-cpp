#include "view/mainwindow_impl.h"
#include "./ui_mainwindow.h"




/*
================================================================================
================================================================================
==
==        LoggerManager
==
================================================================================
================================================================================
*/


MainWindow::LoggerManager::LoggerManager(Ui::MainWindow* uip)
      : ui(uip) {
  connect(this, &LoggerManager::availableLogMsg,
      this, &LoggerManager::writeLogMsg,
      Qt::ConnectionType::QueuedConnection);
}


void MainWindow::LoggerManager::log(const logger::LogMsg& msg) {
  emit availableLogMsg(msg);
}


void MainWindow::LoggerManager::writeLogMsg(const logger::LogMsg& lm) {
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
      comutils::string::format_string("[%s] [%s] %s",
          comutils::time::getftime().c_str(),
          lvlTxt.c_str(),
          lm.msg.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml(
      QString::fromStdString(
          comutils::string::format_string(
              "<p style=\"color:%s;white-space:pre\">",
              color.c_str()))
      + outText + "</p>");
}
