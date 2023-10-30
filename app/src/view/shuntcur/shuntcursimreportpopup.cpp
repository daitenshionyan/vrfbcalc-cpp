#include "view/shuntcur/shuntcursimreportpopup.h"
#include "./ui_shuntcursimreportpopup.h"

#include <utility>

#include <QtConcurrent/qtconcurrentrun.h>

#include "utillib/concur.hpp"
#include "utillib/utils.hpp"
#include "util/qtutilities.hpp"


namespace {


constexpr double kTimeStep = 1;


}


SCSimReportPopup::SCSimReportPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::SCSimReportPopup) {
  ui->setupUi(this);

  ui->voltPlot->setupPlot({}, {}, "Time", "Voltage");

  connect(this, &QDialog::finished,
      this, &SCSimReportPopup::deleteSelf);
  connect(&watcher, &QFutureWatcher<vrfbdriver::ShuntSimStep>::resultReadyAt,
      this, &SCSimReportPopup::reportReadyAt,
      Qt::ConnectionType::QueuedConnection);
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
}


SCSimReportPopup::~SCSimReportPopup() {
  delete ui;
}


void SCSimReportPopup::start(const vrfbdriver::ShuntSimJob& j) {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [j](QPromise<vrfbdriver::ShuntSimStep>& p) {
        auto rpter = vrfbutils::BasePromise_Qt<vrfbdriver::ShuntSimStep>{&p};
        try {
          vrfbdriver::simulateShunt(j, kTimeStep, rpter);
        } catch (...) {
          return;
        }
      }
  ));
}


void SCSimReportPopup::reportReadyAt(int i) {
  auto step = watcher.future().resultAt(i);
  ui->voltPlot->addPoint(
      ui->voltPlot->dataCount(),
      step.report.data<vrfb::shuntcur::pcc::PCCReport>().chargingVolt());
  report.update<vrfb::shuntcur::pcc::PCCReport>(step, kTimeStep);
}


void SCSimReportPopup::displayReport() {
  ui->msgLabel->setText(QString::fromStdString(comutils::string::format_string(
      "Completed : EE=%.2f%%",
      report.energyEff() * 100)));
}
