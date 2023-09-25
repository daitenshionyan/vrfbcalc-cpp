#include "view/celleff/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include <filesystem>

#include "utillib/utils.hpp"
#include "vrfblib/vrfblib.hpp"


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
  plotForms.push_back({"CE", "CE vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kCEHdr),
    ui->ceCycPlot});
  plotForms.push_back({"CE", "CE vs Total time",
    std::string(vrfb::celleff::kTotalTimeHdr),
    std::string(vrfb::celleff::kCEHdr),
    ui->ceTimePlot});
  plotForms.push_back({"EE", "EE vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kEEHdr),
    ui->eeCycPlot});
  plotForms.push_back({"EE", "EE vs Total time",
    std::string(vrfb::celleff::kTotalTimeHdr),
    std::string(vrfb::celleff::kEEHdr),
    ui->eeTimePlot});
  plotForms.push_back({"VE", "VE vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kVEHdr),
    ui->veCycPlot});
  plotForms.push_back({"VE", "VE vs Total time",
    std::string(vrfb::celleff::kTotalTimeHdr),
    std::string(vrfb::celleff::kVEHdr),
    ui->veTimePlot});
  plotForms.push_back({"ASR", "ASR vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kASRHdr),
    ui->asrCycPlot});
  plotForms.push_back({"ASR", "ASR vs Total time",
    std::string(vrfb::celleff::kTotalTimeHdr),
    std::string(vrfb::celleff::kASRHdr),
    ui->asrTimePlot});
  plotForms.push_back({"TIME", "Total time vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kTotalTimeHdr),
    ui->totalTimeCycPlot});
  plotForms.push_back({"TIME", "Cycle time vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kCycleTimeHdr),
    ui->cycTimeCycPlot});
  plotForms.push_back({"TIME", "Chg time vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kChgTimeHdr),
    ui->chgTimeCycPlot});
  plotForms.push_back({"TIME", "DChg time vs Cycle number",
    std::string(vrfb::celleff::kCycleNumberHdr),
    std::string(vrfb::celleff::kDChgTimeHdr),
    ui->dchgTimeCycPlot});
}


void CEResultView::plotGraphs(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  for (const auto& form : plotForms) {
    form.plot->setupPlot(entries, form.xHdr, form.yHdr);
  }
}


bool CEResultView::exportImages(logger::Logger& l) {
  std::string prefix = ui->prefixNameField->text().toStdString();
  if (!comutils::io::isValidFileName(prefix)) {
    l.warn(comutils::string::format_string(
        "Prefix field may contain illegal file characters '%s'",
        prefix.c_str()));
  }
  if (!prefix.empty()) {
    prefix += "_";
  }
  std::filesystem::create_directories("output/images");
  bool is_success = true;
  for (const auto& form : plotForms) {
    QString strPath = QString::fromStdString(comutils::string::format_string(
        "output/images/%s%s_%s.png",
        prefix.c_str(), form.title.c_str(), form.name.c_str()));
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


CEResultView::~CEResultView() {
  delete ui;
}


void CEResultView::on_exportBtn_clicked() {
  emit exportRequested(this);
}
