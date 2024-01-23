#include "view/shuntcur/shuntcursimesipos2configpanel.h"
#include "./ui_shuntcursimesipos2configpanel.h"




SCSimESIPOS2ConfigPanel::SCSimESIPOS2ConfigPanel(QWidget* parent)
      : SCSimConfigPopup::ConfigPanel(parent),
        ui(new Ui::SCSimESIPOS2ConfigPanel) {
  ui->setupUi(this);
}


SCSimESIPOS2ConfigPanel::~SCSimESIPOS2ConfigPanel() {
  delete ui;
}


vrfb::shuntcur::ShuntCalc* SCSimESIPOS2ConfigPanel::getCalc() const {
  return new vrfb::shuntcur::esipos2::ESIPOS2Calc(
      vrfb::shuntcur::esipos2::ESIPOS2SysParam {
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
          vrfb::shuntcur::esipos2::ConnParam {
              ui->connShuntLenInField->value() / 100,
              ui->connShuntAreaField->value() / 10000,
              ui->connManiLenField->value() / 100,
              ui->connManiAreaField->value() / 10000,
              ui->mConnShuntLenInField->value() / 100,
              ui->mConnShuntAreaField->value() / 10000,
              ui->mConnManiLenField->value() / 100,
              ui->mConnManiAreaField->value() / 10000,
              ui->connShuntLenOutField->value() / 100,
              ui->connShuntAreaField->value() / 10000,
              ui->connManiLenField->value() / 100,
              ui->connManiAreaField->value() / 10000,
              ui->mConnShuntLenOutField->value() / 100,
              ui->mConnShuntAreaField->value() / 10000,
              ui->mConnManiLenField->value() / 100,
              ui->mConnManiAreaField->value() / 10000}});
}
