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
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::cellCurrs,
    ui->currentPlot});
  // stack shunts ------------------------------------
  formDatas.push_back({"SPT current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::sptCurrs,
    ui->sptCurrPlot});
  formDatas.push_back({"SPB current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::spbCurrs,
    ui->spbCurrPlot});
  formDatas.push_back({"SNT current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::sntCurrs,
    ui->sntCurrPlot});
  formDatas.push_back({"SNB current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::snbCurrs,
    ui->snbCurrPlot});
  // stack manifolds ---------------------------------
  formDatas.push_back({"MPT current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::mptCurrs,
    ui->mptCurrPlot});
  formDatas.push_back({"MPB current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::mpbCurrs,
    ui->mpbCurrPlot});
  formDatas.push_back({"MNT current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::mntCurrs,
    ui->mntCurrPlot});
  formDatas.push_back({"MNB current",
    IndexingType::itStack,
    &vrfb::shuntcur::scl::SCLReport::mnbCurrs,
    ui->mnbCurrPlot});
  // connector shunts --------------------------------
  formDatas.push_back({"CSPT current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::csptCurrs,
    ui->csptCurrPlot});
  formDatas.push_back({"CSPB current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::cspbCurrs,
    ui->cspbCurrPlot});
  formDatas.push_back({"CSNT current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::csntCurrs,
    ui->csntCurrPlot});
  formDatas.push_back({"CSNB current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::csnbCurrs,
    ui->csnbCurrPlot});
  // connector manifolds -----------------------------
  formDatas.push_back({"CMPT current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::cmptCurrs,
    ui->cmptCurrPlot});
  formDatas.push_back({"CMPB current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::cmpbCurrs,
    ui->cmpbCurrPlot});
  formDatas.push_back({"CMNT current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::cmntCurrs,
    ui->cmntCurrPlot});
  formDatas.push_back({"CMNB current",
    IndexingType::itConn,
    &vrfb::shuntcur::scl::SCLReport::cmnbCurrs,
    ui->cmnbCurrPlot});
}


SCResultView::~SCResultView() {
  delete ui;
}


void SCResultView::plotGraphs(const vrfb::shuntcur::scl::SCLReport& p) {
  std::vector<double> series_xs {};
  std::vector<double> series_xc {};
  for (std::size_t i = 0; i < p.totCells(); ++i) {
    series_xs.push_back(i+1);
  }
  for (std::size_t i = 0; i < p.numStacks(); ++i) {
    series_xc.push_back(i+1);
  }

  for (const auto& form : formDatas) {
    switch (form.it) {
      case IndexingType::itStack:
        form.plot->setupPlot(series_xs, (p.*form.yseriesGetter)(), "Cell No.", "Current (A)");
        break;
      case IndexingType::itConn:
        form.plot->setupPlot(series_xc, (p.*form.yseriesGetter)(), "Stack No.", "Current (A)");
        break;
    }
  }

  ui->chgVoltField->setText(QString::fromStdString(std::to_string(p.chargingVolt())));
  ui->chgCurrField->setText(QString::fromStdString(std::to_string(p.chargingCurr())));
  ui->chgPowrField->setText(QString::fromStdString(std::to_string(p.chargingPowr())));
  ui->ovplField->setText(QString::fromStdString(std::to_string(p.overVoltPowr())));
  ui->inputPowrField->setText(QString::fromStdString(std::to_string(p.storedPowr())));
  ui->energyEffField->setText(QString::fromStdString(std::to_string(p.storedPowr() / p.chargingPowr() * 100)));
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
