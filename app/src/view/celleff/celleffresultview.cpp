#include "view/celleff/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>

#include <filesystem>

#include "utillib/utils.hpp"
#include "vrfblib/vrfblib.hpp"


CEResultView::CEResultView(logger::Logger* logger, QWidget* parent)
      : QDialog(parent),
        l(logger), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished,
      this, &CEResultView::deleteSelf);
  connect(this, &CEResultView::exportRequest,
      this, &CEResultView::handleExportRequest,
      Qt::QueuedConnection);
  connect(this, &CEResultView::exportSuccess,
      this, &CEResultView::handleExportSuccess,
      Qt::QueuedConnection);
  connect(this, &CEResultView::exportFailure,
      this, &CEResultView::handleExportFailure,
      Qt::QueuedConnection);
  // Cell efficiency
  plotCfgs.push_back({"CE vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kCEHdr)});
  plotCfgs.push_back({"CE vs Total time",
      std::string(vrfb::celleff::kTotalTimeHdr),
      std::string(vrfb::celleff::kCEHdr)});
  // Energy efficiency
  plotCfgs.push_back({"EE vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kEEHdr)});
  plotCfgs.push_back({"EE vs Total time",
      std::string(vrfb::celleff::kTotalTimeHdr),
      std::string(vrfb::celleff::kEEHdr)});
  // Voltage efficiency
  plotCfgs.push_back({"VE vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kVEHdr)});
  plotCfgs.push_back({"VE vs Total time",
      std::string(vrfb::celleff::kTotalTimeHdr),
      std::string(vrfb::celleff::kVEHdr)});
  // ASR
  plotCfgs.push_back({"ASR vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kASRHdr)});
  plotCfgs.push_back({"ASR vs Total time",
      std::string(vrfb::celleff::kTotalTimeHdr),
      std::string(vrfb::celleff::kASRHdr)});
  // Time
  plotCfgs.push_back({"Total time vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kTotalTimeHdr)});
  plotCfgs.push_back({"Cycle time vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kCycleTimeHdr)});
  plotCfgs.push_back({"Chg time vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kChgTimeHdr)});
  plotCfgs.push_back({"DChg time vs Cycle number",
      std::string(vrfb::celleff::kCycleNumberHdr),
      std::string(vrfb::celleff::kDChgTimeHdr)});
}


CEResultView::~CEResultView() {
  delete ui;
}


void CEResultView::plotGraphs(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries) {
  // get series names
  std::vector<std::string> nlist {};        // name list
  for (const auto& e : entries) {
    nlist.push_back(e.name);
  }
  // initialize plots
  for (auto& cfg : plotCfgs) {
    cfg.panel = new PlotPanel(this, cfg.title);
    ui->scrollAreaWidgetContents->layout()->addWidget(cfg.panel);
    cfg.panel->setupPlot(cfg.xHdr, cfg.yHdr, nlist);
    // add points to plot
    for (std::size_t i_entry = 0; i_entry < entries.size(); ++i_entry) {
      const auto& e = entries.at(i_entry);
      std::vector<std::pair<double, double>> series {};
      for (std::size_t i = 0; i < e.table.numRows(); ++i) {
        series.push_back({
            e.table.get<double>(cfg.xHdr, i),
            e.table.get<double>(cfg.yHdr, i)});
      }
      cfg.panel->addPoint(series, i_entry);
    }
  }
}


bool CEResultView::exportGraphs(logger::Logger& logger) {
  std::string prefix = ui->prefixNameField->text().toStdString();
  if (!comutils::io::isValidFileName(prefix)) {
    logger.warn(comutils::string::format_string(
        "Prefix field may contain illegal file characters '%s'",
        prefix.c_str()));
  }
  std::filesystem::create_directories("output/images");
  bool is_success = true;
  for (const auto& cfg : plotCfgs) {
    bool is_saved = false;
    try {
      is_saved = cfg.panel->savePng("output/images", prefix);
    } catch (const std::exception& ex) {
      is_success = false;
      logger.fail(comutils::string::format_string("Failed to export '%s' : %s",
          cfg.title.c_str(), ex.what()));
      continue;
    } catch (...) {
      is_success = false;
      logger.fail(comutils::string::format_string("Failed to export '%s'",
          cfg.title.c_str()));
      continue;
    }
    if (is_saved) {
      logger.info(comutils::string::format_string("Successfully exported '%s'",
          cfg.title.c_str()));
    } else {
      is_success = false;
      logger.fail(comutils::string::format_string("Failed to export '%s'",
          cfg.title.c_str()));
    }
  }
  return is_success;
}


void CEResultView::handleExportRequest() {
  if (watcher.isRunning()) {
    l->warn("Already exporting");
    return;
  }
  hide();
  watcher.setFuture(QtConcurrent::run(
      &pool,
      [&](QPromise<void>& p) {
        bool is_success = exportGraphs(*l);
        if (is_success) {
          emit exportSuccess();
        } else {
          emit exportFailure();
        }
      }
  ));
}


void CEResultView::handleExportSuccess() {
  done(QDialog::Accepted);
}


void CEResultView::handleExportFailure() {
  show();
}


void CEResultView::on_exportBtn_clicked() {
  emit exportRequest();
}
