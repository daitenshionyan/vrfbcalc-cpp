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
      ui->asrField->value(),
      ui->cellAreaField->value(),
      ui->shuntLenField->value(),
      ui->shuntAreaField->value(),
      ui->maniLenField->value(),
      ui->maniAreaField->value(),
      ui->resistivityField->value()
    },
    ui->numStacksField->value(),
    ui->conLenField->value(),
    ui->conAreaField->value()
  );
}
