#include "view/shuntcur/shuntcurresultview.h"
#include "./ui_shuntcurresultview.h"

#include <vector>


SCResultView::SCResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &SCResultView::deleteSelf);
}


SCResultView::~SCResultView() {
  delete ui;
}


void SCResultView::plotGraphs(const vrfb::shuntcur::ShuntPerf& p) {
  std::vector<double> series_x {};
  std::vector<double> series_curr {};
  std::vector<double> series_powr {};

  for (std::size_t i = 0; i < p.numCells(); ++i) {
    series_x.push_back(i+1);
    series_curr.push_back(p.cellCurr(i));
    series_powr.push_back(p.cellPowr(i));
  }

  ui->currentPlot->setupPlot(series_x, series_curr, "Cell No.", "Current (A)");
  ui->powerPlot->setupPlot(series_x, series_powr, "Cell No.", "Power (W)");
}
