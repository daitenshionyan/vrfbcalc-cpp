#include "view/mainwindow.h"
#include "./ui_mainwindow.h"
#include "view/mainwindow_impl.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>
#include <QFileDialog>

#include "logger.hpp"
#include "utillib/utils.hpp"
#include "vrfbcalccfg.hpp"
#include "driver/vrfbdriver_io.hpp"


#include "view/celleff/celleffconfigpopup.h"
#include "view/shuntcur/shuntcursimconfigpopup.h"















MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        logger(new LoggerManager(ui)),
        driver(new DriverManager(this, ui, logger)) {
  ui->setupUi(this);
  ui->outputArea->appendHtml(
      QString("<p style=\"color:grey;white-space:pre\">")
      + QString::fromStdString(
            comutils::string::format_string(
                "%s\n%s\n%s",
                "================================================================================",
                vrfbcfg::licence_notice,
                "================================================================================"))
      + QString("</p>"));
}


MainWindow::~MainWindow() {
  delete driver;
  delete logger;
  delete ui;
}


// ---- < SLOTS > --------------------------------------------------------------


void MainWindow::on_action_openOutput_triggered(bool) {
  std::filesystem::path output_path {"output"};
  if (!std::filesystem::exists(output_path)) {
    logger->fail("Output folder does not exist yet");
    return;
  } else if (!std::filesystem::is_directory(output_path)) {
    logger->fail("The file 'output' exists but it is not a directory");
    return;
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile("output"));
}


void MainWindow::on_actionCECalculations_triggered(bool) {
  driver->showConfigPopup_CE();
}


void MainWindow::on_actionCEAnalysis_triggered(bool) {
  driver->analyseResults_CE();
}


void MainWindow::on_actionSCSimulate_triggered(bool) {
  driver->showConfigPopup_SE();
}
