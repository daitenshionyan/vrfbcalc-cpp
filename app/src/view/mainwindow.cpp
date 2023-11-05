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








/*
================================================================================
================================================================================
==
==        AppDriver
==
================================================================================
================================================================================
*/


class MainWindow::AppDriver : public QObject {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    AppDriver(MainWindow* mainWindow)
          : mw(mainWindow) {
      connect(&watcher, &QFutureWatcher<void>::started,
          this, &AppDriver::disableActions);
      connect(&watcher, &QFutureWatcher<void>::canceled,
          this, &AppDriver::enableActions);
      connect(&watcher, &QFutureWatcher<void>::finished,
          this, &AppDriver::enableActions);
      connect(mw, &MainWindow::completedPerformanceReading,
          this, &AppDriver::displayPerformanceView,
          Qt::ConnectionType::QueuedConnection);
      connect(mw, &MainWindow::jobReadySCSim,
          this, &AppDriver::displayReport_SCSim,
          Qt::ConnectionType::QueuedConnection);
    }

    AppDriver() = default;
    AppDriver(const AppDriver&) = default;
    AppDriver(AppDriver&&) = default;

    AppDriver& operator=(const AppDriver&) = default;
    AppDriver& operator=(AppDriver&&) = default;

    ~AppDriver() {
      delete config_ce;
      delete config_scSim;
    }




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void enableActions() {
      mw->ui->menuRun->setDisabled(true);
    }


    void disableActions() {
      mw->ui->menuRun->setDisabled(true);
    }




  public: // ~~~~ cell efficiency functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void showConfigPopup_CE() {
      if (watcher.isRunning()) {
        mw->logger->warn("Another process is already running");
        return;
      }
      if (config_ce == nullptr) {
        constructConfigPopup_CE();
      }
      config_ce->show();
    }


    void analyseResults_CE() {
      if (watcher.isRunning()) {
        mw->logger->warn("Another process is already running");
        return;
      }
      QString openPath = "output";
      if (!std::filesystem::exists(openPath.toStdString())) {
        openPath = QString();
      }
      QStringList qstrPaths = QFileDialog::getOpenFileNames(
          mw,
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
            emit mw->completedPerformanceReading(
                vrfbdriver::readPerformance_CE(strPaths, *(mw->logger)));
          }));
    }


  private:
    void constructConfigPopup_CE() {
      config_ce = new CEConfigPopup(mw);
      connect(config_ce, &QDialog::accepted,
          this, &AppDriver::startCalc_CE);
    }


    void startCalc_CE() {
      watcher.setFuture(QtConcurrent::run(
        &pool,
        [&](QPromise<void>& p) {
          vrfbdriver::calcCellEff(config_ce->getSetSupplierMap(), *(mw->logger));
        }));
    }


    void displayPerformanceView(const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
      if (entries.empty()) {
        return;
      }
      CEResultView* rv = new CEResultView(mw);
      try {
        rv->plotGraphs(entries);
      } catch (std::exception& ex) {
        mw->logger->fail(comutils::string::format_string("Failed to plot graphs due to - %s",
            ex.what()));
        delete rv;
        return;
      }
      connect(rv, &CEResultView::exportRequested,
          this, &AppDriver::exportCEPerformance);
      rv->open();
    }


    void exportCEPerformance(CEResultView* rv) {
      if (watcher.isRunning()) {
        mw->logger->warn("Cannot export as another process is already running");
        return;
      }
      rv->hide();
      watcher.setFuture(QtConcurrent::run(
          &pool,
          [&, rv](QPromise<void>& p) {
            bool is_success = rv->exportImages(*(mw->logger));
            if (is_success) {
              rv->done(QDialog::Accepted);
            } else {
              rv->show();
            }
          }
      ));
    }




  public: // ~~~~ shunt current functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void showConfigPopup_SE() {
      if (config_scSim == nullptr) {
        constructConfigPopup_SC();
      }
      config_scSim->show();
    }


  private:
    void constructConfigPopup_SC() {
      config_scSim = new SCSimConfigPopup(mw);
      connect(config_scSim, &QDialog::accepted,
          this, &AppDriver::startSim_SC);
    }


    void startSim_SC() {
      watcher.setFuture(QtConcurrent::run(
          &pool,
          [&](QPromise<void>& p) {
            try {
              auto job = config_scSim->getJob();
              emit mw->jobReadySCSim(job);
            } catch (...) {
              mw->logger->fail("Simulation failed");
              return;
            }
          }
      ));
    }


    void displayReport_SCSim(const vrfbdriver::shuntcur::ShuntSimJob& j) {
      SCSimReportPopup* popup = new SCSimReportPopup(mw);
      try {
        popup->show();
        popup->start(j);
      } catch (...) {
        mw->logger->fail("Failed to simulate");
        delete popup;
        return;
      }
    }




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    MainWindow* mw;
    CEConfigPopup* config_ce = nullptr;
    SCSimConfigPopup* config_scSim = nullptr;

    QFutureWatcher<void> watcher;
    QThreadPool pool;
};















MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        logger(new LoggerManager(ui)),
        driver(new MainWindow::AppDriver(this)) {
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
