#include "view/graphplotformtitled.h"
#include "./ui_graphplotformtitled.h"


GraphPlotFormTitled::GraphPlotFormTitled(const std::string& title, QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphPlotFormTitled) {
  ui->setupUi(this);
  ui->titleLabel->setText(QString::fromStdString(title));
}


GraphPlotFormTitled::~GraphPlotFormTitled() {
  delete ui;
}


void GraphPlotFormTitled::setupPlot(
    const std::vector<double>& series_x,
    const std::vector<double>& series_y,
    const std::string& name_x, const std::string& name_y) {
  ui->plot->setupPlot(series_x, series_y, name_x, name_y);
}


void GraphPlotFormTitled::setupPlot(const std::vector<GraphPlotForm::Series>& slist,
    const std::string& name_x, const std::string& name_y) {
  ui->plot->setupPlot(slist, name_x, name_y);
}


bool GraphPlotFormTitled::savePng(const QString& path) {
  return ui->plot->savePng(path);
}
