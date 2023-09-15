#include "view/graphplotform.h"
#include "./ui_graphplotform.h"

#include <cmath>
#include <limits>
#include <vector>


namespace { // BEGIN OF NAMESPACE <GLOBAL::UNNAMED> ============================


constexpr double kZeroAreaPortion = 0.2;
constexpr double kEndSpacePortion = 0.1;


double correctLowerValue(double lv, double uv, double corrDiff, int pow) {
  double fac = std::pow(10, pow);
  int clv = std::floor(std::ceil(uv * fac) - corrDiff);
  clv += 10 - std::abs(clv % 10);
  if (clv > std::ceil(lv * fac)) {
    clv = std::ceil(lv * fac);
    clv -= std::abs(clv % 10);
  }
  return clv / fac;
}


double correctUpperValue(double lv, double uv, double corrDiff, int pow) {
  double fac = std::pow(10, pow);
  int cuv = std::floor(std::ceil(lv * fac) + corrDiff);
  cuv -= std::abs(cuv % 10);
  if (cuv < std::ceil(uv * fac)) {
    cuv = std::ceil(uv * fac);
    cuv += 10 - std::abs(cuv % 10);
  }
  return cuv / fac;
}


void adjustAxisRange(QCPAxis* axis) {
  auto r = axis->range();
  double diff = r.upper - r.lower;
  if (diff == 0) {
    return;
  }

  // find inverse power to get ceil of diff to at least a 100
  // 100 so that modulo operator works
  int pow = 0;
  for (double num = diff; std::ceil(num) < 10; num *= 10) {
    ++pow;
  }

  double lower = r.lower;
  double upper = r.upper;
  double corrDiff = std::ceil(diff * std::pow(10, pow)) * (1+kEndSpacePortion);
  if (upper < 0 || lower < 0) {
    // correct lower
    lower = correctLowerValue(r.lower, r.upper, corrDiff, pow);
    // correct upper
    if (diff*kZeroAreaPortion > -upper || upper == 0) {
      upper = 0;
    } else {
      upper = correctUpperValue(r.lower, r.upper, corrDiff, pow);
    }
  } else {
    // correct lower
    if (diff*kZeroAreaPortion > lower || lower == 0) {
      lower = 0;
    } else {
      lower = correctLowerValue(r.lower, r.upper, corrDiff, pow);
    }
    // correct upper
    upper = correctUpperValue(r.lower, r.upper, corrDiff, pow);
  }

  axis->setRange(lower, upper);
}


} // END OF NAMESPACE <GLOBAL::UNNAMED> ----------------------------------------
// namespace <GLOBAL>


GraphPlotForm::GraphPlotForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::GraphPlotForm) {
  ui->setupUi(this);
}


GraphPlotForm::~GraphPlotForm() {
  delete ui;
}


void GraphPlotForm::initialiseData(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries,
      const std::string& xHdr, const std::string& yHdr) {
  double min_x = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();

  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  ui->plot->plotLayout()->addElement(0, 1, subLayout);
  ui->plot->plotLayout()->addElement(0, 2, new QCPLayoutElement);
  subLayout->addElement(0, 0, new QCPLayoutElement);
  subLayout->addElement(1, 1, ui->plot->legend);
  subLayout->setRowStretchFactor(1, 0.001);
  subLayout->addElement(2, 2, new QCPLayoutElement);
  ui->plot->plotLayout()->setColumnStretchFactor(1, 0.001);
  ui->plot->plotLayout()->setColumnStretchFactor(2, 0.001);
  ui->plot->legend->setVisible(true);

  std::size_t i_series = 0;
  for (const vrfbdriver::PerformanceEntry_CE& entry : entries) {
    auto graph = ui->plot->addGraph();

    // set graph style
    QColor color {};
    color.setHsvF((float) ((double) i_series / entries.size()), 1, 0.7);
    graph->setPen(color);
    graph->setName(QString::fromStdString(entry.name));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCross));
    graph->setAntialiased(true);

    // add graph data
    for (std::size_t i = 0; i < entry.table.numRows(); ++i) {
      double x = entry.table.get<double>(xHdr, i);
      double y = entry.table.get<double>(yHdr, i);
      graph->addData(x, y);
      min_x = (min_x < x) ? min_x : x;
      max_x = (max_x < x) ? x : max_x;
      min_y = (min_y < y) ? min_y : y;
      max_y = (max_y < y) ? y : max_y;
    }
    ++i_series;
  }

  // set initial range
  ui->plot->xAxis->setRange(min_x, max_x);
  ui->plot->yAxis->setRange(min_y, max_y);
  adjustAxisRange(ui->plot->xAxis);
  adjustAxisRange(ui->plot->yAxis);
  ui->plot->xAxis->setLabel(QString::fromStdString(xHdr));
  ui->plot->yAxis->setLabel(QString::fromStdString(yHdr));
  ui->plot->replot();

  // initialize range field values
  auto xr = ui->plot->xAxis->range();
  auto yr = ui->plot->yAxis->range();
  ui->xAxisLowerField->setValue(xr.lower);
  ui->xAxisUpperField->setValue(xr.upper);
  ui->yAxisLowerField->setValue(yr.lower);
  ui->yAxisUpperField->setValue(yr.upper);
}


bool GraphPlotForm::savePng(const QString& path) {
  return ui->plot->savePng(path);
}


// ---- < SLOTS > --------------------------------------------------------------


void GraphPlotForm::on_xAxisLowerField_valueChanged(double value) {
  ui->plot->xAxis->setRangeLower(value);
  ui->plot->replot();
}


void GraphPlotForm::on_xAxisUpperField_valueChanged(double value) {
  ui->plot->xAxis->setRangeUpper(value);
  ui->plot->replot();
}


void GraphPlotForm::on_yAxisLowerField_valueChanged(double value) {
  ui->plot->yAxis->setRangeLower(value);
  ui->plot->replot();
}


void GraphPlotForm::on_yAxisUpperField_valueChanged(double value) {
  ui->plot->yAxis->setRangeUpper(value);
  ui->plot->replot();
}
