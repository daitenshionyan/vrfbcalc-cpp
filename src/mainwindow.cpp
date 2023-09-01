#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}


MainWindow::~MainWindow() {
  delete ui;
}


void MainWindow::on_startBtn_clicked() {
  vrfbdriver::calcCellEff_a("CellEffConfig.json", *this);
}


void MainWindow::write(const std::string& text) {
  ui->outputArea->insertPlainText(text.c_str());
}

void MainWindow::writeln(const std::string& text) {
  ui->outputArea->appendPlainText(text.c_str());
}
