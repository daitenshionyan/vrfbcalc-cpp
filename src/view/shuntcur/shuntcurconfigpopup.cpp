#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"

#include "view/shuntcur/shuntcurconfigformstack.h"


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
  form = new SCConfigFormStack(this);
  ui->formArea->layout()->addWidget(form);
}


SCConfigPopup::~SCConfigPopup() {
  delete form;
  delete ui;
}


SCConfigPopup::Job SCConfigPopup::getJob() {
  return {
    ui->nameField->text().toStdString(),
    ui->chgCurrField->value(),
    form->getGenerator()};
}
