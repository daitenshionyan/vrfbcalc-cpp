#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
}


SCConfigPopup::~SCConfigPopup() {
  delete ui;
}


vrfbdriver::ShuntJob SCConfigPopup::getJob() {
  vrfb::shuntcur::CommLineCalc* calc = new vrfb::shuntcur::CommLineCalc(
    {
      (std::size_t) ui->numCellField->value(),
      ui->resistivityField->value(),
      ui->asrField->value() / 10000,
      ui->cellAreaField->value() / 10000,
      ui->shuntLenField->value() / 100,
      ui->shuntAreaField->value() / 10000,
      ui->maniLenField->value() / 100,
      ui->maniAreaField->value() / 10000
    },
    ui->numStackField->value(),
    {
      ui->resistivityField->value(),
      ui->connShuntLenField->value() / 100,
      ui->connShuntAreaField->value() / 10000,
      ui->connManiLenField->value() / 100,
      ui->connManiAreaField->value() / 10000
    });
  return vrfbdriver::ShuntJob(
      ui->nameField->text().toStdString(),
      calc,
      ui->chgVoltField->value());
}
