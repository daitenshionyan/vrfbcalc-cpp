#include "view/shuntcur/shuntcurconfigformstack.h"
#include "./ui_shuntcurconfigformstack.h"


SCConfigFormStack::SCConfigFormStack(QWidget* parent)
    : SCConfigPopup::ConfigForm(parent), ui(new Ui::SCConfigFormStack) {
  ui->setupUi(this);
}


SCConfigFormStack::~SCConfigFormStack() {
  delete ui;
}


vrfb::shuntcur::ParamGenerator* SCConfigFormStack::getGenerator() {
  ui->numCellField->value();
  return new vrfb::shuntcur::StackArrGenerator(
    {
      static_cast<std::size_t>(ui->numCellField->value()),
      ui->asrField->value() / 10000,
      ui->cellAreaField->value() / 10000,
      ui->shuntLenField->value() / 100,
      ui->shuntAreaField->value() / 10000,
      ui->maniLenField->value() / 100,
      ui->maniAreaField->value() / 10000,
      ui->resistivityField->value()
    },
    ui->numStacksField->value(),
    ui->conLenField->value() / 10000,
    ui->conAreaField->value() / 10000
  );
}
