#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>
#include <QFileDialog>

#include "utillib/utils.hpp"
#include "vrfbcalccfg.hpp"
#include "driver/vrfbdriver_io.hpp"

#include "view/shuntcur/shuntcurresultview.h"


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
        popup_ce(new CEConfigPopup(this)),
        popup_se(new SCConfigPopup(this)) {
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
  connect(this, &MainWindow::availableLogMsg,
      this, &MainWindow::logMsg);
  connect(popup_ce, &CEConfigPopup::accepted,
      this, &MainWindow::startCalc_CE);
  connect(popup_se, &SCConfigPopup::accepted,
      this, &MainWindow::startCalc_SC);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::resultReadyAt,
      this, &MainWindow::logMsgAt);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::started,
      this, &MainWindow::disableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::canceled,
      this, &MainWindow::enableActions);
  connect(&watcher, &QFutureWatcher<logger::LogMsg>::finished,
      this, &MainWindow::enableActions);
  connect(this, &MainWindow::completedPerformanceReading,
      this, &MainWindow::displayPerformanceView,
      Qt::ConnectionType::QueuedConnection);
  connect(this, &MainWindow::completedSCCalc,
      this, &MainWindow::displayPerformanceView_SC,
      Qt::ConnectionType::QueuedConnection);
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete popup_se;
  delete ui;
}


void MainWindow::startCalc_CE() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<logger::LogMsg>& p) {
        auto w = PromLogger{p};
        vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), w);
      }));
}


void MainWindow::startCalc_SC() {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<logger::LogMsg>& p) {
        auto w = PromLogger{p};
        try {
          auto res = vrfbdriver::calcShuntPerf(popup_se->getJob(), w);
          emit completedSCCalc(res);
        } catch (std::exception& ex) {
          w.fail(comutils::string::format_string("Error while calculating shunt performance: %s",
              ex.what()));
          return;
        }
      }
  ));
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
      comutils::string::format_string("[%s] [%s] %s",
          comutils::time::getftime().c_str(),
          lvlTxt.c_str(),
          lm.msg.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml(
      QString::fromStdString(
          comutils::string::format_string("<p style=\"color:%s;white-space:pre\">",
              color.c_str()))
      + outText + "</p>");
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
    fail(comutils::string::format_string("Failed to plot graphs due to - %s",
        ex.what()));
    delete rv;
    return;
  }
  connect(rv, &CEResultView::exportRequested,
      this, &MainWindow::exportCEPerformance);
  rv->open();
}


void MainWindow::displayPerformanceView_SC(const vrfbdriver::ShuntRes& r) {
  switch (r.arrType) {
    case vrfbdriver::SCArrType::scatSCL: {
      SCResultView* rv = new SCResultView(this, r.name);
      try {
        rv->plotGraphs(r.perf.data<vrfb::shuntcur::scl::SCLReport>());
      } catch (std::exception& ex) {
        fail(comutils::string::format_string("Failed to plot graphs due to - %s",
            ex.what()));
        delete rv;
        return;
      }
      connect(rv, &SCResultView::exportRequested,
          this, &MainWindow::exportSEPerformance);
      rv->open();
      break;
    }
    case vrfbdriver::SCArrType::scatPCC: {
      SCReportPopup* rp = new SCReportPopup(r.name, this);
      try {
        rp->plotGraphs(r.perf.data<vrfb::shuntcur::pcc::PCCReport>(),
            {
              {
                "Cell current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::cellCurr
              },
              {
                "SPT current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::sptCurr
              },
              {
                "SPB current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::spbCurr
              },
              {
                "SNT current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::sntCurr
              },
              {
                "SNB current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::snbCurr
              },
              {
                "MPT current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::mptCurr
              },
              {
                "MPB current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::mpbCurr
              },
              {
                "MNT current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::mntCurr
              },
              {
                "MNB current",
                SCReportPopup::IndexingType::itCell,
                &vrfb::shuntcur::pcc::PCCReport::mnbCurr
              }
            });
      } catch (std::exception& ex) {
        fail(comutils::string::format_string("Failed to plot graphs due to - %s",
            ex.what()));
        delete rp;
        return;
      }
      connect(rp, &SCReportPopup::exportRequested,
          this, &MainWindow::exportSCReport);
      rp->open();
      break;
    }
  }
}


void MainWindow::exportCEPerformance(CEResultView* rv) {
  if (watcher.isRunning()) {
    warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        bool is_success = rv->exportImages(l);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}


void MainWindow::exportSEPerformance(SCDataView* rv) {
  if (watcher.isRunning()) {
    warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        bool is_success = rv->exportImages(l);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}


void MainWindow::exportSCReport(SCReportPopup* rv) {
  if (watcher.isRunning()) {
    warn("Cannot export as another process is already running");
    return;
  }
  rv->hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&, rv](QPromise<logger::LogMsg>& p) {
        auto l = PromLogger{p};
        bool is_success = rv->exportImages(l);
        if (is_success) {
          rv->done(QDialog::Accepted);
        } else {
          rv->show();
        }
      }
  ));
}


// ---- < SLOTS > --------------------------------------------------------------


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


void MainWindow::on_actionCECalculations_triggered(bool) {
  if (watcher.isRunning()) {
    warn("Cannot perform calculations as another process is already running");
    return;
  }
  popup_ce->exec();
}


void MainWindow::on_actionCEAnalysis_triggered(bool) {
  if (watcher.isRunning()) {
    warn("Cannot analyse data as another process is already running");
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
        auto l = PromLogger{p};
        std::vector<std::string> strPaths {};
        for (const QString& qstrPath : qstrPaths) {
          strPaths.push_back(qstrPath.toStdString());
        }
        emit completedPerformanceReading(vrfbdriver::readPerformance_CE(strPaths, l));
      }));
}


void MainWindow::on_actionSCCalculations_triggered(bool) {
  popup_se->open();
}
