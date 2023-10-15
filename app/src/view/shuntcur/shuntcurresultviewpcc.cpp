#include "view/shuntcur/shuntcurresultviewpcc.h"
#include "./ui_shuntcurresultviewpcc.h"


SCResultView_PCC::SCResultView_PCC(QWidget* parent, const std::string& p)
    : MainWindow::SCDataView(parent), ui(new Ui::SCResultView_PCC), prefix(p) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &SCResultView_PCC::deleteSelf);
  addPlot("Cell current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::cellCurr);
  // stack shunts ------------------------------------
  addPlot("SPT current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::sptCurr);
  addPlot("SPB current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::spbCurr);
  addPlot("SNT current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::sntCurr);
  addPlot("SNB current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::snbCurr);
  // stack manifolds ---------------------------------
  addPlot("MPT current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::mptCurr);
  addPlot("MPB current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::mpbCurr);
  addPlot("MNT current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::mntCurr);
  addPlot("MNB current",
      IndexingType::itStack,
      &vrfb::shuntcur::pcc::PCCReport::mnbCurr);
}


SCResultView_PCC::~SCResultView_PCC() {
  delete ui;
}


void SCResultView_PCC::plotGraphs(const vrfb::shuntcur::pcc::PCCReport& p) {
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
      case IndexingType::itStack: {
        std::vector<GraphPlotForm::Series> slist {};
        for (std::size_t li = 0; li < p.numLines(); ++li) {
          slist.push_back({"Line " + std::to_string(li)});
          for (std::size_t i = 0; i < p.numCells()*p.numStacks(); ++i) {
            slist.back().xs.push_back(i);
            slist.back().ys.push_back((p.*form.yseriesGetter)(li*p.numCells()*p.numStacks() + i));
          }
        }
        form.plot->setupPlot(slist, "Cell No.", "Current (A)");
        break;
      }
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


bool SCResultView_PCC::exportImages(logger::Logger& l) {
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


void SCResultView_PCC::on_exportBtn_clicked() {
  emit exportRequested(this);
}


void SCResultView_PCC::addPlot(const std::string& name, IndexingType it, SeriesGetter getter) {
  GraphPlotFormTitled* plot = new GraphPlotFormTitled(name, this);
  ui->scrollAreaContents->layout()->addWidget(plot);
  formDatas.push_back({name, it, getter, plot});
}
