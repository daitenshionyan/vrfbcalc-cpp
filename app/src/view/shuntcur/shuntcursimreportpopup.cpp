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
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimStep>::resultReadyAt,
      this, &SCSimReportPopup::reportReadyAt);
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimStep>::progressValueChanged,
      ui->progressBar, &QProgressBar::setValue,
      Qt::ConnectionType::QueuedConnection);
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimJob>::progressRangeChanged,
      ui->progressBar, &QProgressBar::setRange,
      Qt::ConnectionType::QueuedConnection);
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimStep>::progressTextChanged,
      ui->msgLabel, &QLabel::setText,
      Qt::ConnectionType::QueuedConnection);
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimStep>::finished,
      this, &SCSimReportPopup::displayReport,
      Qt::ConnectionType::QueuedConnection);
  connect(this, &SCSimReportPopup::simulationFailed,
      this, &SCSimReportPopup::displayFailed,
      Qt::ConnectionType::QueuedConnection);
}


SCSimReportPopup::~SCSimReportPopup() {
  delete ui;
}


void SCSimReportPopup::start(const vrfbdriver::ShuntSimJob& j) {
  watcher.setFuture(QtConcurrent::run<vrfbdriver::ShuntSimStep>(
      &pool,
      [&, j](QPromise<vrfbdriver::ShuntSimStep>& p) {
        auto rpter = vrfbutils::BasePromise_Qt<vrfbdriver::ShuntSimStep>{&p};
        try {
          vrfbdriver::simulateShunt(j, kTimeStep, rpter);
        } catch (const std::exception& ex) {
          emit simulationFailed(ex.what());
          throw vrfbutils::StdQException(ex);
        } catch (...) {
          emit simulationFailed("Unknown exception occured");
          throw vrfbutils::StdQException(std::runtime_error("Unknown exception occured"));
        }
      }
  ));
}


void SCSimReportPopup::reportReadyAt(int i) {
  auto step = watcher.future().resultAt(i);
  ui->voltPlot->addPoint<PlotPanel::Axis::axMain>(
      step.time,
      step.report.data<vrfb::shuntcur::pcc::PCCReport>().chargingVolt());
  ui->voltPlot->addPoint<PlotPanel::Axis::axSub>(
      step.time,
      step.report.data<vrfb::shuntcur::pcc::PCCReport>().chargingCurr());
  report.update<vrfb::shuntcur::pcc::PCCReport>(step, kTimeStep);
}


void SCSimReportPopup::displayReport() {
  if (watcher.isCanceled()) {
    return;
  }
  ui->msgLabel->setText(QString::fromStdString(comutils::string::format_string(
      "Completed : EE=%.2f%%",
      report.energyEff() * 100)));
}


void SCSimReportPopup::displayFailed(const std::string& msg) {
  ui->progressBar->setStyleSheet(
      "QProgressBar { background: #FC3B3B; }"
      "QProgressBar::chunk { background: #FC3B3B; }");
  ui->msgLabel->setText(QString::fromStdString(comutils::string::format_string(
      "Simulation Failed - %s",
      msg.c_str())));
}
