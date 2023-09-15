#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include <cmath>
#include <limits>
#include <vector>

#include "qcustomplot.h"

#include "driver/vrfbdriver_io.hpp"


namespace { // BEGIN OF NAMESPACE <GLOBAL::UNNAMED> ============================


constexpr double kZeroAreaPortion = 0.2;
constexpr double kEndSpacePortion = 0.1;


double correctLowerValue(double lv, double uv, double corrDiff, int pow) {
  double clv = std::ceil(uv * std::pow(10, pow)) - corrDiff;
  clv = (clv + (10 - std::abs((int) clv % 10))) / std::pow(10, pow);
  if (clv > lv) {
    clv = std::ceil(lv * std::pow(10, pow));
    clv = (clv - std::abs((int) clv % 10)) / std::pow(10, pow);
  }
  return clv;
}


double correctUpperValue(double lv, double uv, double corrDiff, int pow) {
  double cuv = std::ceil(lv * std::pow(10, pow)) + corrDiff;
  cuv = (cuv - std::abs((int) cuv % 10)) / std::pow(10, pow);
  if (cuv < uv) {
    cuv = std::ceil(uv * std::pow(10, pow));
    cuv = (cuv + (10 + std::abs((int) cuv % 10))) / std::pow(10, pow);
  }
  return cuv;
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


void plotGraph(
      QCustomPlot* plot,
      const std::vector<vrfb::Table>& tables,
      const std::string& xHdr,
      const std::string& yHdr) {
  double min_x = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();

  std::size_t i_series = 0;
  for (const vrfb::Table& table : tables) {
    auto graph = plot->addGraph();

    // set graph style
    QColor color {};
    color.setHsvF((float) ((double) i_series / tables.size()), 1, 0.7);
    graph->setPen(color);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCross));
    graph->setAntialiased(true);

    // add graph data
    for (std::size_t i = 0; i < table.numRows(); ++i) {
      double x = table.get<double>(xHdr, i);
      double y = table.get<double>(yHdr, i);
      graph->addData(x, y);
      min_x = (min_x < x) ? min_x : x;
      max_x = (max_x < x) ? x : max_x;
      min_y = (min_y < y) ? min_y : y;
      max_y = (max_y < y) ? y : max_y;
    }
    ++i_series;
  }

  // set initial range
  plot->xAxis->setRange(min_x, max_x);
  plot->yAxis->setRange(min_y, max_y);
  adjustAxisRange(plot->xAxis);
  adjustAxisRange(plot->yAxis);
  plot->xAxis->setLabel(QString::fromStdString(xHdr));
  plot->yAxis->setLabel(QString::fromStdString(yHdr));
  plot->replot();
}


} // END OF NAMESPACE <GLOBAL::UNNAMED> ----------------------------------------


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
}


void CEResultView::plotGraphs(const std::vector<vrfb::Table>& tables) {
  QCustomPlot* customPlot = new QCustomPlot();
  plotGraph(ui->ceCycPlot, tables, "Cycle No.", "CE (Fractional)");
  plotGraph(ui->ceTimePlot, tables, "Total Time (s)", "CE (Fractional)");
}


CEResultView::~CEResultView() {
  delete ui;
}
