#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QDesktopServices>

#include "strutils.hpp"
#include "vrfbcalccfg.hpp"


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
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete ui;
}


void MainWindow::on_action_openOutput_triggered(bool) {
  std::filesystem::path output_path {"output"};
  if (!std::filesystem::exists(output_path)) {
    writeln_fail("Output folder does not exist yet");
    return;
  } else if (!std::filesystem::is_directory(output_path)) {
    writeln_fail("The file 'output' exists but it is not a directory");
    return;
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile("output"));
}


void MainWindow::on_startBtn_clicked() {
  vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), *this);
}


void MainWindow::on_cfgBtn_clicked() {
  popup_ce->exec();
}


void MainWindow::writeln(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:black;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_succ(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:green;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_warn(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:orange;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_fail(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:red;white-space:pre\">" + outText + "</p>");
}
