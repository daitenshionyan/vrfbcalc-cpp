#include "view/shuntcur/shuntcursimesiposconfigpanel.h"
#include "./ui_shuntcursimesiposconfigpanel.h"




SCSimESIPOSConfigPanel::SCSimESIPOSConfigPanel(QWidget* parent)
      : SCSimConfigPopup::ConfigPanel(parent),
        ui(new Ui::SCSimESIPOSConfigPanel) {
  ui->setupUi(this);
}


SCSimESIPOSConfigPanel::~SCSimESIPOSConfigPanel() {
  delete ui;
}


vrfb::shuntcur::ShuntCalc* SCSimESIPOSConfigPanel::getCalc() const {
  return new vrfb::shuntcur::esipos::ESIPOSCalc(
      vrfb::shuntcur::esipos::ESIPOSSysParam {
          vrfb::shuntcur::SysParam {
              vrfb::shuntcur::StackParam {
                  ui->asrField->value() / 10000,
                  ui->cellAreaField->value() / 10000,
                  ui->shuntLenField->value() / 100,
                  ui->shuntAreaField->value() / 10000,
                  ui->maniLenField->value() / 100,
                  ui->maniAreaField->value() / 10000},
              ui->resistivityField->value(),
              ui->maxChgDenField->value() * 10,
              (std::size_t) ui->numCellField->value(),
              (std::size_t) ui->numStackField->value(),
              (std::size_t) ui->numLineField->value(),
              0.1,                                                // SOC
              ui->tempField->value() + 273.15},
          vrfb::shuntcur::esipos::ConnParam {
              ui->connShuntLenField->value() / 100,
              ui->connShuntAreaField->value() / 10000,
              ui->connManiLenField->value() / 100,
              ui->connManiAreaField->value() / 10000,
              ui->mConnShuntLenField->value() / 100,
              ui->mConnShuntAreaField->value() / 10000,
              ui->mConnManiLenField->value() / 100,
              ui->mConnManiAreaField->value() / 10000,
              ui->connShuntLenField->value() / 100,
              ui->connShuntAreaField->value() / 10000,
              ui->connManiLenField->value() / 100,
              ui->connManiAreaField->value() / 10000}});
}
