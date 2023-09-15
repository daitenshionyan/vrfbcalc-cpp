#include "view/celleffresultview.h"
#include "./ui_celleffresultview.h"


CEResultView::CEResultView(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEResultView) {
  ui->setupUi(this);
  connect(this, &QDialog::finished, this, &CEResultView::deleteSelf);
}


void CEResultView::plotGraphs(const std::vector<vrfb::Table>& tables) {
  ui->ceCycPlot->initialiseData(tables, "Cycle No.", "CE (Fractional)");
  ui->ceTimePlot->initialiseData(tables, "Total Time (s)", "CE (Fractional)");
}


CEResultView::~CEResultView() {
  delete ui;
}
