#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>
#include <QFileDialog>

#include "logger.hpp"
#include "utillib/utils.hpp"
#include "vrfbcalccfg.hpp"
#include "driver/vrfbdriver_io.hpp"


/*
================================================================================
================================================================================
==
==        AppLogger
==
================================================================================
================================================================================
*/


class MainWindow::AppLogger
      : public QObject, public logger::Logger {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    AppLogger(MainWindow* mainWindow)
          : mw(mainWindow) {
      connect(mw, &MainWindow::availableLogMsg,
          this, &AppLogger::writeLogMsg,
          Qt::ConnectionType::QueuedConnection);
    }

    AppLogger() = default;
    AppLogger(const AppLogger&) = default;
    AppLogger(AppLogger&&) = default;

    AppLogger& operator=(const AppLogger&) = default;
    AppLogger& operator=(AppLogger&&) = default;

    ~AppLogger() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void log(const logger::LogMsg& msg) override {
      emit mw->availableLogMsg(msg);
    }


  private:
    void writeLogMsg(const logger::LogMsg& lm) {
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
      mw->ui->outputArea->appendHtml(
          QString::fromStdString(
              comutils::string::format_string(
                  "<p style=\"color:%s;white-space:pre\">",
                  color.c_str()))
          + outText + "</p>");
    }


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    MainWindow* mw;
};















MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        logger(new MainWindow::AppLogger(this)),
        popup_ce(new CEConfigPopup(this)),
        popup_scsim(new SCSimConfigPopup(this)) {
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
  connect(popup_ce, &CEConfigPopup::accepted,
      this, &MainWindow::startCalc_CE);
  connect(popup_scsim, &SCSimConfigPopup::accepted,
      this, &MainWindow::startSim_SC);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::started,
      this, &MainWindow::disableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::canceled,
      this, &MainWindow::enableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::finished,
      this, &MainWindow::enableActions);
  connect(this, &MainWindow::completedPerformanceReading,
      this, &MainWindow::displayPerformanceView,
      Qt::ConnectionType::QueuedConnection);
  connect(this, &MainWindow::jobReadySCSim,
      this, &MainWindow::displayReport_SCSim,
      Qt::ConnectionType::QueuedConnection);
}


MainWindow::~MainWindow() {
  delete logger;
  delete popup_ce;
  delete popup_scsim;
  delete ui;
}


void MainWindow::startCalc_CE() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<logger::LogMsg>& p) {
        vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), *logger);
      }));
}


void MainWindow::startSim_SC() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<logger::LogMsg>& p) {
        try {
          auto job = popup_scsim->getJob();
          emit jobReadySCSim(job);
        } catch (...) {
          logger->fail("Simulation failed");
          return;
        }
      }
  ));
}


void MainWindow::disableActions() {
  ui->menuRun->setDisabled(true);
}


void MainWindow::enableActions() {
  ui->menuRun->setDisabled(false);
}


void MainWindow::displayPerformanceView(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  if (entries.empty()) {
    return;
  }
  CEResultView* rv = new CEResultView(this);
  try {
    rv->plotGraphs(entries);
  } catch (std::exception& ex) {
    logger->fail(comutils::string::format_string("Failed to plot graphs due to - %s",
        ex.what()));
    delete rv;
    return;
  }
  connect(rv, &CEResultView::exportRequested,
      this, &MainWindow::exportCEPerformance);
  rv->open();
}


void MainWindow::exportCEPerformance(CEResultView* rv) {
  if (watcher.isRunning()) {
    logger->warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<logger::LogMsg>& p) {
        bool is_success = rv->exportImages(*logger);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}


void MainWindow::displayReport_SCSim(const vrfbdriver::shuntcur::ShuntSimJob& j) {
  SCSimReportPopup* popup = new SCSimReportPopup(this);
  try {
    popup->show();
    popup->start(j);
  } catch (...) {
    logger->fail("Failed to simulate");
    delete popup;
    return;
  }
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
  if (watcher.isRunning()) {
    logger->warn("Cannot perform calculations as another process is already running");
    return;
  }
  popup_ce->exec();
}


void MainWindow::on_actionCEAnalysis_triggered(bool) {
  if (watcher.isRunning()) {
    logger->warn("Cannot analyse data as another process is already running");
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
        std::vector<std::string> strPaths {};
        for (const QString& qstrPath : qstrPaths) {
          strPaths.push_back(qstrPath.toStdString());
        }
        emit completedPerformanceReading(vrfbdriver::readPerformance_CE(strPaths, *logger));
      }));
}


void MainWindow::on_actionSCSimulate_triggered(bool) {
  popup_scsim->open();
}
