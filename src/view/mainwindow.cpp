#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include "strutils.hpp"


MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        popup_ce(new CEConfigPopup(this)) {
  ui->setupUi(this);
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete ui;
}


void MainWindow::on_startBtn_clicked() {
  vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), *this);
}


void MainWindow::on_cfgBtn_clicked() {
  popup_ce->exec();
}


void MainWindow::writeln(const std::string& text) {
  ui->outputArea->appendHtml(QString::fromStdString(strutils::format_string(
      "<font color=black>[%s] %s</font>",
      strutils::getftime().c_str(), text.c_str())));
}


void MainWindow::writeln_succ(const std::string& text) {
  ui->outputArea->appendHtml(QString::fromStdString(strutils::format_string(
      "<font color=green>[%s] %s</font>",
      strutils::getftime().c_str(), text.c_str())));
}


void MainWindow::writeln_warn(const std::string& text) {
  ui->outputArea->appendHtml(QString::fromStdString(strutils::format_string(
      "<font color=orange>[%s] %s</font>",
      strutils::getftime().c_str(), text.c_str())));
}


void MainWindow::writeln_fail(const std::string& text) {
  ui->outputArea->appendHtml(QString::fromStdString(strutils::format_string(
      "<font color=red>[%s] %s</font>",
      strutils::getftime().c_str(), text.c_str())));
}
