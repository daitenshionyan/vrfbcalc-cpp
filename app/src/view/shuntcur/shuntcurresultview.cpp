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

  for (std::size_t i = 0; i < p.totCells(); ++i) {
    series_x.push_back(i+1);
  }

  ui->currentPlot->setupPlot(series_x, p.cellCurrs(), "Cell No.", "Current (A)");
  ui->sptCurrPlot->setupPlot(series_x, p.sptCurrs(), "Cell No.", "Current (A)");
  ui->spbCurrPlot->setupPlot(series_x, p.spbCurrs(), "Cell No.", "Current (A)");
  ui->sntCurrPlot->setupPlot(series_x, p.sntCurrs(), "Cell No.", "Current (A)");
  ui->snbCurrPlot->setupPlot(series_x, p.snbCurrs(), "Cell No.", "Current (A)");
  ui->mptCurrPlot->setupPlot(series_x, p.mptCurrs(), "Cell No.", "Current (A)");
  ui->mpbCurrPlot->setupPlot(series_x, p.mpbCurrs(), "Cell No.", "Current (A)");
  ui->mntCurrPlot->setupPlot(series_x, p.mntCurrs(), "Cell No.", "Current (A)");
  ui->mnbCurrPlot->setupPlot(series_x, p.mnbCurrs(), "Cell No.", "Current (A)");

  ui->chgVoltField->setText(QString::fromStdString(std::to_string(p.chargingVolt())));
  ui->chgCurrField->setText(QString::fromStdString(std::to_string(p.chargingCurr())));
  ui->chgPowrField->setText(QString::fromStdString(std::to_string(p.chargingPowr())));
  ui->inputPowrField->setText(QString::fromStdString(std::to_string(p.totalPowr())));
  ui->energyEffField->setText(QString::fromStdString(std::to_string(p.totalPowr() / p.chargingPowr() * 100)));
}
