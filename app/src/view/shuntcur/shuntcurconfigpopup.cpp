#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"

#include <stdexcept>


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
}


SCConfigPopup::~SCConfigPopup() {
  delete ui;
}


vrfbdriver::ShuntJob SCConfigPopup::getJob() {
  if (ui->nameField->text().isEmpty()) {
    throw std::runtime_error("Blank name");
  }
  if (ui->numStackField->value() <= 0) {
    throw std::runtime_error("Negative or 0 number of stacks");
  }
  if (ui->numCellField->value() <= 0) {
    throw std::runtime_error("Negative or 0 number of cells");
  }
  vrfb::shuntcur::scl::SCLCalc* calc = new vrfb::shuntcur::scl::SCLCalc(
    {
      (std::size_t) ui->numStackField->value(),
      (std::size_t) ui->numCellField->value(),
      ui->resistivityField->value(),
      ui->maxChgDenField->value() * 10,   // mA cm-2    ->      A m-2
      {
        ui->asrField->value() / 10000,
        ui->cellAreaField->value() / 10000,
        ui->shuntLenField->value() / 100,
        ui->shuntAreaField->value() / 10000,
        ui->maniLenField->value() / 100,
        ui->maniAreaField->value() / 10000
      },
      {
        ui->connShuntLenField->value() / 100,
        ui->connShuntAreaField->value() / 10000,
        ui->connManiLenField->value() / 100,
        ui->connManiAreaField->value() / 10000
      }
    });
  return vrfbdriver::ShuntJob(
      ui->nameField->text().toStdString(),
      calc,
      ui->chgVoltField->value());
}
