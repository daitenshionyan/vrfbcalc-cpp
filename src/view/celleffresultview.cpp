#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include "strutils.hpp"


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
  plotForms.push_back({"CE vs Cycle number", ui->ceCycPlot});
  plotForms.push_back({"CE vs Total time", ui->ceTimePlot});
}


void CEResultView::plotGraphs(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  ui->ceCycPlot->setupPlot(entries, "Cycle No.", "CE (Fractional)");
  ui->ceTimePlot->setupPlot(entries, "Total Time (s)", "CE (Fractional)");
}


bool CEResultView::exportImages(logger::Logger& l) {
  std::string prefix = ui->prefixNameField->text().toStdString();
  if (!strutils::isValidFileName(prefix)) {
    l.warn(strutils::format_string(
        "Prefix field may contain illegal file characters '%s'",
        prefix.c_str()));
  }
  bool is_success = true;
  for (const auto& form : plotForms) {
    QString strPath = QString::fromStdString(strutils::format_string(
        "output/images/%s_%s.png",
        prefix.c_str(), form.first.c_str()));
    bool is_saved = form.second->savePng(strPath);
    if (is_saved) {
      l.info(strutils::format_string("Successfully exported '%s'",
          form.first.c_str()));
    } else {
      is_success = false;
      l.fail(strutils::format_string("Failed to export '%s'",
          form.first.c_str()));
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
