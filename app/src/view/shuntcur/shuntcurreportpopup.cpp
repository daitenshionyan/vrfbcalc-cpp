#include "view/shuntcur/shuntcurreportpopup.h"
#include "./ui_shuntcurreportpopup.h"

#include <filesystem>


SCReportPopup::SCReportPopup(const std::string& name, QWidget* parent)
    : QDialog(parent), ui(new Ui::SCReportPopup), prefix{name} {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &SCReportPopup::deleteSelf);
}


SCReportPopup::~SCReportPopup() {
  delete ui;
}


bool SCReportPopup::exportImages(logger::Logger& l) {
  std::filesystem::create_directories("output/images");
  bool is_success = true;
  for (const auto& p : plots) {
    QString strPath = QString::fromStdString(comutils::string::format_string(
          "output/images/%s_%s.png",
          prefix.c_str(), p.name.c_str()));
    bool is_saved = p.plot->savePng(strPath);
    if (is_saved) {
      l.info(comutils::string::format_string("Successfully exported '%s'",
          p.name.c_str()));
    } else {
      is_success = false;
      l.fail(comutils::string::format_string("Failed to export '%s'",
          p.name.c_str()));
    }
  }
  return is_success;
}


void SCReportPopup::initSummary(const vrfb::shuntcur::ShuntReportData& r) {
  ui->arrField->setText(QString::fromStdString(r.arrName()));
  ui->tempField->setText(QString::fromStdString(std::to_string(r.param().temperature() - 273.15)));
  ui->socField->setText(QString::fromStdString(std::to_string(r.param().soc() * 100)));
  ui->chgVoltField->setText(QString::fromStdString(std::to_string(r.chargingVolt())));
  ui->chgCurrField->setText(QString::fromStdString(std::to_string(r.chargingCurr())));
  ui->chgPowrField->setText(QString::fromStdString(std::to_string(r.chargingPowr())));
  ui->ovplField->setText(QString::fromStdString(std::to_string(r.overVoltPowr())));
  ui->inputPowrField->setText(QString::fromStdString(std::to_string(r.storedPowr())));
  ui->energyEffField->setText(QString::fromStdString(std::to_string(r.storedPowr() / r.chargingPowr() * 100)));
}


void SCReportPopup::initPlot(const std::vector<GraphPlotForm::Series>& slist,
      const std::string& name_x, const std::string& name_y,
      const std::string& name) {
  GraphPlotFormTitled* plot = new GraphPlotFormTitled(name, this);
  ui->scrollAreaContents->layout()->addWidget(plot);
  plot->setupPlot(slist, name_x, name_y);
  plots.push_back(Plot{name, plot});
}


void SCReportPopup::on_exportBtn_clicked() {
  emit exportRequested(this);
}








/*
********************************************************************************
**    getPlotConfig
********************************************************************************
*/



// :::: [ SCL ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
std::vector<SCReportPopup::PlotConfig<vrfb::shuntcur::scl::SCLReport>>
SCReportPopup::getPlotConfig<vrfb::shuntcur::scl::SCLReport>() {
  return {
      {
        "Cell current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::cellCurr
      },
      // stack shunts --------------------------------------
      {
        "SPT current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::sptCurr
      },
      {
        "SPB current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::spbCurr
      },
      {
        "SNT current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::sntCurr
      },
      {
        "SNB current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::snbCurr
      },
      // stack manifold ------------------------------------
      {
        "MPT current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::mptCurr
      },
      {
        "MPB current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::mpbCurr
      },
      {
        "MNT current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::mntCurr
      },
      {
        "MNB current",
        IndexingType::itCell,
        &vrfb::shuntcur::scl::SCLReport::mnbCurr
      },
      // connector shunts ----------------------------------
      {
        "CSPT current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::csptCurr
      },
      {
        "CSPB current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::cspbCurr
      },
      {
        "CSNT current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::csntCurr
      },
      {
        "CSNB current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::csnbCurr
      },
      // connector manifold --------------------------------
      {
        "CMPT current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::cmptCurr
      },
      {
        "CMPB current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::cmpbCurr
      },
      {
        "CMNT current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::cmntCurr
      },
      {
        "CMNB current",
        IndexingType::itStack,
        &vrfb::shuntcur::scl::SCLReport::cmnbCurr
      }
  };
}

template
std::vector<SCReportPopup::PlotConfig<vrfb::shuntcur::scl::SCLReport>>
SCReportPopup::getPlotConfig<vrfb::shuntcur::scl::SCLReport>();


// :::: [ PCC ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


template<>
std::vector<SCReportPopup::PlotConfig<vrfb::shuntcur::pcc::PCCReport>>
SCReportPopup::getPlotConfig<vrfb::shuntcur::pcc::PCCReport>() {
  return {
      {
        "Cell current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::cellCurr
      },
      // stack shunts --------------------------------------
      {
        "SPT current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::sptCurr
      },
      {
        "SPB current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::spbCurr
      },
      {
        "SNT current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::sntCurr
      },
      {
        "SNB current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::snbCurr
      },
      // stack manifold ------------------------------------
      {
        "MPT current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::mptCurr
      },
      {
        "MPB current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::mpbCurr
      },
      {
        "MNT current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::mntCurr
      },
      {
        "MNB current",
        IndexingType::itCell,
        &vrfb::shuntcur::pcc::PCCReport::mnbCurr
      }
  };
}

template
std::vector<SCReportPopup::PlotConfig<vrfb::shuntcur::pcc::PCCReport>>
SCReportPopup::getPlotConfig<vrfb::shuntcur::pcc::PCCReport>();
