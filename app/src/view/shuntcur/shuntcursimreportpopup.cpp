#include "view/shuntcur/shuntcursimreportpopup.h"
#include "./ui_shuntcursimreportpopup.h"

#include <QtConcurrent/qtconcurrentrun.h>


namespace {


class ShuntSimReporter_Impl : public vrfbdriver::ShuntSimReporter {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    ShuntSimReporter_Impl(QPromise<vrfb::shuntcur::ShuntReport>* p)
          : prom_p{p} {}

    ShuntSimReporter_Impl() = delete;
    ShuntSimReporter_Impl(const ShuntSimReporter_Impl&) = delete;
    ShuntSimReporter_Impl(ShuntSimReporter_Impl&&) = default;

    ShuntSimReporter_Impl& operator=(const ShuntSimReporter_Impl&) = delete;
    ShuntSimReporter_Impl& operator=(ShuntSimReporter_Impl&&) = default;

    ~ShuntSimReporter_Impl() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void addShuntReport(const vrfb::shuntcur::ShuntReport& r) override {
      prom_p->addResult(r);
    }


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QPromise<vrfb::shuntcur::ShuntReport>* prom_p;
};


}


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
        auto rpter = ShuntSimReporter_Impl{&p};
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
