#include "view/mainwindow_impl.h"
#include "./ui_mainwindow.h"

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>
#include <QFileDialog>
































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
































/*
================================================================================
================================================================================
==
==        DriverManager
==
================================================================================
================================================================================
*/


MainWindow::DriverManager::DriverManager(
      QWidget* parent, Ui::MainWindow* uip, logger::Logger* logger)
      : QObject(parent), p(parent), ui(uip), l(logger) {
  connect(&watcher, &QFutureWatcher<void>::started,
      this, &MainWindow::DriverManager::disableActions);
  connect(&watcher, &QFutureWatcher<void>::canceled,
      this, &MainWindow::DriverManager::enableActions);
  connect(&watcher, &QFutureWatcher<void>::finished,
      this, &MainWindow::DriverManager::enableActions);
  connect(this, &MainWindow::DriverManager::completedPerformanceReading,
      this, &MainWindow::DriverManager::displayPerformanceView,
      Qt::ConnectionType::QueuedConnection);
  connect(this, &MainWindow::DriverManager::jobReadySCSim,
      this, &MainWindow::DriverManager::displayReport_SCSim,
      Qt::ConnectionType::QueuedConnection);
}


MainWindow::DriverManager::~DriverManager() {
  delete config_ce;
  delete config_scSim;
}








/*
********************************************************************************
**        Misc. Functions
********************************************************************************
*/


void MainWindow::DriverManager::enableActions() {
  ui->menuRun->setDisabled(false);
}


void MainWindow::DriverManager::disableActions() {
  ui->menuRun->setDisabled(true);
}








/*
********************************************************************************
**        Cell Efficiency Functions
********************************************************************************
*/


void MainWindow::DriverManager::showConfigPopup_CE() {
  if (watcher.isRunning()) {
    l->warn("Another process is already running");
    return;
  }
  if (config_ce == nullptr) {
    constructConfigPopup_CE();
  }
  config_ce->show();
}


void MainWindow::DriverManager::analyseResults_CE() {
  if (watcher.isRunning()) {
        l->warn("Another process is already running");
        return;
      }
      QString openPath = "output";
      if (!std::filesystem::exists(openPath.toStdString())) {
        openPath = QString();
      }
      QStringList qstrPaths = QFileDialog::getOpenFileNames(
          p,
          "Select one or more performance files to open",
          openPath,
          "XLSX Files (*.xlsx)"
      );
      watcher.setFuture(QtConcurrent::run(
          &pool,
          [&, qstrPaths](QPromise<void>& p) {
            std::vector<std::string> strPaths {};
            for (const QString& qstrPath : qstrPaths) {
              strPaths.push_back(qstrPath.toStdString());
            }
            emit completedPerformanceReading(
                vrfbdriver::readPerformance_CE(strPaths, *l));
          }));
}


void MainWindow::DriverManager::constructConfigPopup_CE() {
  config_ce = new CEConfigPopup(p);
  connect(config_ce, &QDialog::accepted,
      this, &MainWindow::DriverManager::startCalc_CE);
}


void MainWindow::DriverManager::startCalc_CE() {
  watcher.setFuture(QtConcurrent::run(
    &pool,
    [&](QPromise<void>& p) {
      vrfbdriver::calcCellEff(config_ce->getSetSupplierMap(), *l);
    }));
}


void MainWindow::DriverManager::displayPerformanceView(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  if (entries.empty()) {
    return;
  }
  CEResultView* rv = new CEResultView(p);
  try {
    rv->plotGraphs(entries);
  } catch (std::exception& ex) {
    l->fail(comutils::string::format_string("Failed to plot graphs due to - %s",
        ex.what()));
    delete rv;
    return;
  }
  connect(rv, &CEResultView::exportRequested,
      this, &MainWindow::DriverManager::exportCEPerformance);
  rv->open();
}


void MainWindow::DriverManager::exportCEPerformance(CEResultView* rv) {
  if (watcher.isRunning()) {
    l->warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<void>& p) {
        bool is_success = rv->exportImages(*l);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}








/*
********************************************************************************
**        Cell Efficiency Functions
********************************************************************************
*/


void MainWindow::DriverManager::showConfigPopup_SE() {
  if (config_scSim == nullptr) {
    constructConfigPopup_SC();
  }
  config_scSim->show();
}


void MainWindow::DriverManager::constructConfigPopup_SC() {
  config_scSim = new SCSimConfigPopup(p);
  connect(config_scSim, &QDialog::accepted,
      this, &MainWindow::DriverManager::startSim_SC);
}


void MainWindow::DriverManager::startSim_SC() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<void>& p) {
        try {
          auto job = config_scSim->getJob();
          emit jobReadySCSim(job);
        } catch (...) {
          l->fail("Simulation failed");
          return;
        }
      }
  ));
}


void MainWindow::DriverManager::displayReport_SCSim(
      const vrfbdriver::shuntcur::ShuntSimJob& j) {
  SCSimReportPopup* popup = new SCSimReportPopup(p);
  try {
    popup->show();
    popup->start(j);
  } catch (...) {
    l->fail("Failed to simulate");
    delete popup;
    return;
  }
}
