#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include <cmath>
#include <exception>
#include <filesystem>
#include <vector>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "qcustomplot.h"

#include "driver/vrfbdriver_io.hpp"


namespace { // BEGIN OF NAMESPACE <GLOBAL::UNNAMED> ============================


constexpr int kIntervalFactor = 5;
constexpr int kMaxHorizontalTick = 11;


int calcInterval(int diff, int targetCount = kMaxHorizontalTick) {
  int interval = (int) std::ceil((double) diff / (targetCount+1));
  if (interval > kIntervalFactor) {
    interval += interval % kIntervalFactor;
  }
  return interval;
}


QValueAxis* createAxisWhole(int min, int max) {
  QValueAxis* axis = new QValueAxis();
  int diff = max - min;
  if (diff <= 0) {
    return axis;
  }
  int interval = calcInterval(diff);
  int tickCount = (int) std::ceil((double) diff / interval) + 1;
  int maxRange = min + interval * (tickCount-1);
  axis->setRange(min, maxRange);
  axis->setTickInterval(interval);
  axis->setTickCount(tickCount);
  axis->setLabelFormat("%d");
  return axis;
}


QValueAxis* createAxisDouble(double min, double max, int targetCount) {
  QValueAxis* axis = new QValueAxis();
  double diff = max - min;
  if (diff <= 0) {
    return axis;
  }
  int pow = 0;
  for (double num = diff; std::ceil(num) < targetCount-1; num *= 10) {
    ++pow;
  }
  double interval = calcInterval((int) (diff * std::pow(10, pow)), targetCount)
      / std::pow(10, pow);
  int tickCount = (int) std::ceil(diff / interval) + 1;
  double maxRange = min + interval * (tickCount-1);
  axis->setRange(min, maxRange);
  axis->setTickInterval(interval);
  axis->setTickCount(tickCount);
  return axis;
}


QChartView* createChart_CE(const std::vector<vrfb::Table>& tables) {
  QChart* chart = new QChart();
  std::size_t max_cyc_num = 0;
  double max_ce = 0;
  for (const vrfb::Table& table : tables) {
    QLineSeries* series = new QLineSeries();
    for (std::size_t i = 0; i < table.numRows(); ++i) {
      int cyc_num = table.get<int>("Cycle No.", i);
      double ce = table.get<double>("CE (Fractional)", i);
      series->append(cyc_num, ce);
      max_cyc_num = (max_cyc_num < cyc_num) ? cyc_num : max_cyc_num;
      max_ce = (max_ce < ce) ? ce : max_ce;
    }
    chart->addSeries(series);
  }
  QValueAxis* xAx = createAxisWhole(0, max_cyc_num);
  QValueAxis* yAx = createAxisDouble(0.5, max_ce, 11);
  chart->setAxisX(xAx);
  chart->setAxisY(yAx);
  for (auto s : chart->series()) {
    s->attachAxis(xAx);
    s->attachAxis(yAx);
  }
  QChartView* chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  return chartView;
}


}


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
}


void CEResultView::createGraphs(const std::vector<vrfb::Table>& tables) {
  QCustomPlot* customPlot = new QCustomPlot();
  ui->ceArea->layout()->addWidget(customPlot);
}


CEResultView::~CEResultView() {
  delete ui;
}
