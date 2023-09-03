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
  ui->outputArea->clear();
  auto cfgMap = popup_ce->getDataSets();

  int num_err = 0;
  for (const auto entry : cfgMap) {
    num_err += calcCellEff_s(entry.first, entry.second, *this);
  }

  writeln(strutils::format_string("\n>>> Total = %d || Success = %d || Failure = %d",
      cfgMap.size(), cfgMap.size()-num_err, num_err));
}


void MainWindow::on_cfgBtn_clicked() {
  popup_ce->exec();
}


void MainWindow::write(const std::string& text) {
  ui->outputArea->insertPlainText(text.c_str());
}


void MainWindow::writeln(const std::string& text) {
  ui->outputArea->appendPlainText(text.c_str());
}
