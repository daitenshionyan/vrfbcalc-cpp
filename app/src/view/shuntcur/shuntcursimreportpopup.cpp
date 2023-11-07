#include "view/shuntcur/shuntcursimreportpopup.h"
#include "./ui_shuntcursimreportpopup.h"

#include <utility>

#include <QtConcurrent/qtconcurrentrun.h>

#include "utillib/concur.hpp"
#include "utillib/utils.hpp"
#include "util/qtutilities.hpp"
#include "view/plotpanel.h"


namespace {


constexpr double kTimeStep = 1;


}


SCSimReportPopup::SCSimReportPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::SCSimReportPopup) {
  ui->setupUi(this);

  ui->voltPlot->setupPlot(
      "Time (s)",
      "Voltage (V)", std::vector<std::string> {"Voltage"},
      "Current (A)", std::vector<std::string> {"Current"});

  connect(this, &QDialog::finished,
      this, &SCSimReportPopup::deleteSelf);
  connect(&watcher, &QFutureWatcher<vrfbdriver::shuntcur::ShuntSimStep>::resultReadyAt,
      this, &SCSimReportPopup::reportReadyAt);
  connect(&watcher, &QFutureWatcher<vrfbdriver::shuntcur::ShuntSimStep>::progressValueChanged,
      ui->progressBar, &QProgressBar::setValue);
  connect(&watcher, &QFutureWatcher<vrfbdriver::shuntcur::ShuntSimJob>::progressRangeChanged,
      ui->progressBar, &QProgressBar::setRange);
  connect(&watcher, &QFutureWatcher<vrfbdriver::shuntcur::ShuntSimStep>::progressTextChanged,
      ui->msgLabel, &QLabel::setText);
  connect(this, &SCSimReportPopup::simulationSuccess,
      this, &SCSimReportPopup::displayReport,
      Qt::ConnectionType::QueuedConnection);
  connect(this, &SCSimReportPopup::simulationFailed,
      this, &SCSimReportPopup::displayFailed,
      Qt::ConnectionType::QueuedConnection);
}


SCSimReportPopup::~SCSimReportPopup() {
  delete ui;
}


void SCSimReportPopup::start(const vrfbdriver::shuntcur::ShuntSimJob& j) {
  watcher.setFuture(QtConcurrent::run<vrfbdriver::shuntcur::ShuntSimStep>(
      &pool,
      [&, j](QPromise<vrfbdriver::shuntcur::ShuntSimStep>& p) {
        auto rpter = vrfbutils::BasePromise_Qt<vrfbdriver::shuntcur::ShuntSimStep>{&p};
        try {
          vrfbdriver::shuntcur::simulateShunt(j, kTimeStep, rpter);
        } catch (const std::exception& ex) {
          emit simulationFailed(ex.what());
          return;
        } catch (...) {
          emit simulationFailed("Unknown exception occured");
          return;
        }
        emit simulationSuccess();
      }
  ));
}


void SCSimReportPopup::reportReadyAt(int i) {
  auto step = watcher.future().resultAt(i);
  ui->voltPlot->addPoint<PlotPanel::Axis::axMain>(
      step.time,
      step.report.data<vrfb::shuntcur::ShuntReportData>().chargingVolt());
  ui->voltPlot->addPoint<PlotPanel::Axis::axSub>(
      step.time,
      step.report.data<vrfb::shuntcur::ShuntReportData>().chargingCurr());
  report.update<vrfb::shuntcur::ShuntReportData>(step, kTimeStep);
}


void SCSimReportPopup::displayReport() {
  ui->progressBar->setStyleSheet(
      "QProgressBar { background: #75F281; }"
      "QProgressBar::chunk { background: #75F281; }");
  ui->msgLabel->setText(QString::fromStdString(comutils::string::format_string(
      "Completed : EE=%.2f%%",
      report.energyEff() * 100)));
}


void SCSimReportPopup::displayFailed(const std::string& msg) {
  ui->progressBar->setStyleSheet(
      "QProgressBar { background: #F25771; }"
      "QProgressBar::chunk { background: #F25771; }");
  ui->msgLabel->setText(QString::fromStdString(comutils::string::format_string(
      "Simulation Failed - %s",
      msg.c_str())));
}


void SCSimReportPopup::deleteSelf(int) {
  watcher.cancel();
  watcher.waitForFinished();
  delete this;
}
