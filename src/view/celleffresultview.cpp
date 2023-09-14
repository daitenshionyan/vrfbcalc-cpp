#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"

#include <exception>
#include <filesystem>
#include <vector>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "driver/vrfbdriver_io.hpp"


namespace {


QChartView* createChart_CE(const std::vector<vrfb::Table>& tables) {
  QChart* chart = new QChart();
  for (const vrfb::Table& table : tables) {
    QLineSeries* series = new QLineSeries();
    for (std::size_t i = 0; i < table.numRows(); ++i) {
      series->append(
          table.get<int>("Cycle No.", i),
          table.get<double>("CE (Fractional)", i));
    }
    chart->addSeries(series);
  }
  chart->createDefaultAxes();
  QChartView* chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  return chartView;
}


}


CEResultView::CEResultView(
    QWidget* parent, const QStringList& files)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  std::vector<vrfb::Table> tables {};
  for (const QString& file : files) {
    std::filesystem::path fp = std::filesystem::u8path<std::string>(file.toStdString());
    tables.push_back(vrfbdriver::io::readTable_XLSX(fp, "Data"));
  }
  ui->ceArea->layout()->addWidget(createChart_CE(tables));
}


CEResultView::~CEResultView() {
  delete ui;
}
