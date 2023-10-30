#include "view/shuntcur/shuntcursimreportpopup.h"
#include "./ui_shuntcursimreportpopup.h"

#include <utility>

#include <QtConcurrent/qtconcurrentrun.h>

#include "utillib/concur.hpp"
#include "util/qtutilities.hpp"


SCSimReportPopup::SCSimReportPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::SCSimReportPopup) {
  ui->setupUi(this);

  ui->voltPlot->setupPlot({}, {}, "Time", "Voltage");

  connect(&watcher, &QFutureWatcher<vrfb::shuntcur::ShuntReport>::resultReadyAt,
      this, &SCSimReportPopup::reportReadyAt);
  connect(this, &QDialog::finished,
      this, &SCSimReportPopup::deleteSelf);
}


SCSimReportPopup::~SCSimReportPopup() {
  delete ui;
}


void SCSimReportPopup::start(const vrfbdriver::ShuntSimJob& j) {
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [j](QPromise<vrfb::shuntcur::ShuntReport>& p) {
        auto rpter = vrfbutils::BasePromise_Qt<vrfb::shuntcur::ShuntReport>{&p};
        try {
          vrfbdriver::simulateShunt(j, rpter);
        } catch (...) {
          return;
        }
      }
  ));
}


void SCSimReportPopup::reportReadyAt(int i) {
  auto rpt = watcher.future().resultAt(i);
  ui->voltPlot->addPoint(
      ui->voltPlot->dataCount(),
      rpt.data<vrfb::shuntcur::pcc::PCCReport>().chargingVolt());
}
