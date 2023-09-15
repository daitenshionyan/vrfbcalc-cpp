#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::accepted, this, &CEResultView::exportPng);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
}


void CEResultView::plotGraphs(const std::vector<vrfb::Table>& tables) {
  ui->ceCycPlot->initialiseData(tables, "Cycle No.", "CE (Fractional)");
  ui->ceTimePlot->initialiseData(tables, "Total Time (s)", "CE (Fractional)");
}


void CEResultView::exportPng() {
  ui->ceCycPlot->savePng("output/CE-Cycle Num Plot.png");
  ui->ceTimePlot->savePng("output/CE-Time Plot.png");
}


CEResultView::~CEResultView() {
  delete ui;
}


void CEResultView::on_exportBtn_clicked() {
  this->accept();
}
