#include "view/shuntcur/shuntcurresultview.h"
#include "./ui_shuntcurresultview.h"

#include <filesystem>
#include <vector>

#include "utillib/utils.hpp"


SCResultView::SCResultView(QWidget* parent, const std::string& p)
    : QDialog(parent), ui(new Ui::SCResultView), prefix{p} {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &SCResultView::deleteSelf);
  formDatas.push_back({"Cell current",
    &vrfb::shuntcur::ShuntPerf::cellCurrs,
    ui->currentPlot});
  // stack shunts
  formDatas.push_back({"SPT current",
    &vrfb::shuntcur::ShuntPerf::sptCurrs,
    ui->sptCurrPlot});
  formDatas.push_back({"SPB current",
    &vrfb::shuntcur::ShuntPerf::spbCurrs,
    ui->spbCurrPlot});
  formDatas.push_back({"SNT current",
    &vrfb::shuntcur::ShuntPerf::sntCurrs,
    ui->sntCurrPlot});
  formDatas.push_back({"SNB current",
    &vrfb::shuntcur::ShuntPerf::snbCurrs,
    ui->snbCurrPlot});
  // stack manifolds
  formDatas.push_back({"MPT current",
    &vrfb::shuntcur::ShuntPerf::mptCurrs,
    ui->mptCurrPlot});
  formDatas.push_back({"MPB current",
    &vrfb::shuntcur::ShuntPerf::mpbCurrs,
    ui->mpbCurrPlot});
  formDatas.push_back({"MNT current",
    &vrfb::shuntcur::ShuntPerf::mntCurrs,
    ui->mntCurrPlot});
  formDatas.push_back({"MNB current",
    &vrfb::shuntcur::ShuntPerf::mnbCurrs,
    ui->mnbCurrPlot});
}


SCResultView::~SCResultView() {
  delete ui;
}


void SCResultView::plotGraphs(const vrfb::shuntcur::ShuntPerf& p) {
  std::vector<double> series_x {};
  for (std::size_t i = 0; i < p.totCells(); ++i) {
    series_x.push_back(i+1);
  }

  for (const auto& form : formDatas) {
    form.plot->setupPlot(series_x, (p.*form.yseriesGetter)(), "Cell No.", "Current (A)");
  }

  ui->chgVoltField->setText(QString::fromStdString(std::to_string(p.chargingVolt())));
  ui->chgCurrField->setText(QString::fromStdString(std::to_string(p.chargingCurr())));
  ui->chgPowrField->setText(QString::fromStdString(std::to_string(p.chargingPowr())));
  ui->inputPowrField->setText(QString::fromStdString(std::to_string(p.totalPowr())));
  ui->energyEffField->setText(QString::fromStdString(std::to_string(p.totalPowr() / p.chargingPowr() * 100)));
  ui->errorField->setText(QString::fromStdString(std::to_string(p.err())));
}


bool SCResultView::exportImages(logger::Logger& l) {
  std::filesystem::create_directories("output/images");
  bool is_success = true;
  for (const auto& form : formDatas) {
    QString strPath = QString::fromStdString(comutils::string::format_string(
          "output/images/%s_%s.png",
          prefix.c_str(), form.name.c_str()));
    bool is_saved = form.plot->savePng(strPath);
    if (is_saved) {
      l.info(comutils::string::format_string("Successfully exported '%s'",
          form.name.c_str()));
    } else {
      is_success = false;
      l.fail(comutils::string::format_string("Failed to export '%s'",
          form.name.c_str()));
    }
  }
  return is_success;
}


void SCResultView::on_exportBtn_clicked() {
  emit exportRequested(this);
}
