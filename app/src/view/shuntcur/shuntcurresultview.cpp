#include "view/shuntcur/shuntcurresultview.h"
#include "./ui_shuntcurresultview.h"

#include <filesystem>
#include <vector>

#include "utillib/utils.hpp"


SCResultView::SCResultView(QWidget* parent, const std::string& p)
    : QDialog(parent), ui(new Ui::SCResultView), prefix{p} {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &SCResultView::deleteSelf);
  addPlot("Cell current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::cellCurrs);
  // stack shunts ------------------------------------
  addPlot("SPT current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::sptCurrs);
  addPlot("SPB current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::spbCurrs);
  addPlot("SNT current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::sntCurrs);
  addPlot("SNB current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::snbCurrs);
  // stack manifolds ---------------------------------
  addPlot("MPT current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::mptCurrs);
  addPlot("MPB current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::mpbCurrs);
  addPlot("MNT current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::mntCurrs);
  addPlot("MNB current",
      IndexingType::itStack,
      &vrfb::shuntcur::scl::SCLReport::mnbCurrs);
  // connector shunts --------------------------------
  addPlot("CSPT current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::csptCurrs);
  addPlot("CSPB current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::cspbCurrs);
  addPlot("CSNT current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::csntCurrs);
  addPlot("CSNB current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::csnbCurrs);
  // connector manifolds -----------------------------
  addPlot("CMPT current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::cmptCurrs);
  addPlot("CMPB current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::cmpbCurrs);
  addPlot("CMNT current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::cmntCurrs);
  addPlot("CMNB current",
      IndexingType::itConn,
      &vrfb::shuntcur::scl::SCLReport::cmnbCurrs);
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

  ui->arrField->setText(QString::fromStdString(p.arrName()));
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


void SCResultView::addPlot(const std::string& name, IndexingType it, SeriesGetter getter) {
  GraphPlotFormTitled* plot = new GraphPlotFormTitled(name, this);
  ui->scrollAreaContents->layout()->addWidget(plot);
  formDatas.push_back({name, it, getter, plot});
}
