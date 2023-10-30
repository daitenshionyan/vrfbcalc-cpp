#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "./ui_shuntcursimconfigpopup.h"

#include <stdexcept>


SCSimConfigPopup::SCSimConfigPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::SCSimConfigPopup) {
  ui->setupUi(this);
  ui->arrComboBox->insertItem(1, "PCC FB");
  // add values to elec input mode combo box
  ui->chgModeComboBox->insertItem(1, "Const Volt");
  ui->chgModeComboBox->insertItem(2, "Const Curr");
  ui->dchgModeComboBox->insertItem(1, "Const Volt");
  ui->dchgModeComboBox->insertItem(2, "Const Curr");
}


SCSimConfigPopup::~SCSimConfigPopup() {
  delete ui;
}


vrfbdriver::ShuntSimJob SCSimConfigPopup::getJob() {
  if (ui->nameField->text().isEmpty()) {
    throw std::runtime_error("Blank name");
  }
  if (ui->numStackField->value() <= 0) {
    throw std::runtime_error("Negative or 0 number of stacks");
  }
  if (ui->numCellField->value() <= 0) {
    throw std::runtime_error("Negative or 0 number of cells");
  }
  if (ui->arrComboBox->currentIndex() < 0) {
    throw std::runtime_error("Arrange not set");
  }

  return vrfbdriver::ShuntSimJob {
    ui->nameField->text().toStdString(),
    getCalc(),
    ui->volField->value(),
    ui->concField->value(),
    getChgInput(),
    getDChgInput(),
    ui->begSOCField->value() / 100,
    ui->endSOCField->value() / 100,
    static_cast<vrfbdriver::SCArrangement>(ui->arrComboBox->currentIndex())
  };
}









/*
********************************************************************************
**        Accessors
********************************************************************************
*/


vrfb::shuntcur::ShuntCalc* SCSimConfigPopup::getCalc() const {
  vrfbdriver::SCArrangement arr = static_cast<vrfbdriver::SCArrangement>(
      ui->arrComboBox->currentIndex());
  switch (arr) {
    case vrfbdriver::SCArrangement::scaPCCFB:
      return new vrfb::shuntcur::pcc::PCCCalc(
          vrfb::shuntcur::pcc::PCCSysParam {
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
              vrfb::shuntcur::pcc::ConnParam {
                  ui->connShuntLenField->value() / 100,
                  ui->connShuntAreaField->value() / 10000,
                  ui->connManiLenField->value() / 100,
                  ui->connManiAreaField->value() / 10000,
                  ui->mConnShuntLenField->value() / 100,
                  ui->mConnShuntAreaField->value() / 10000,
                  ui->mConnManiLenField->value() / 100,
                  ui->mConnManiAreaField->value() / 10000}},
          vrfb::shuntcur::pcc::PCCCalc::ConnType::ctFB);
      break;
    default:
      throw std::runtime_error("Unsupported arrangement");
  }
}


vrfb::shuntcur::ElecInput SCSimConfigPopup::getChgInput() const {
  if (ui->chgModeComboBox->currentIndex() < 0) {
    throw std::runtime_error("Charging input mode not selected");
  }
  return vrfb::shuntcur::ElecInput {
      static_cast<vrfb::shuntcur::ElecInput::Mode>(ui->chgModeComboBox->currentIndex()),
      ui->chgInputMagField->value()
  };
}


vrfb::shuntcur::ElecInput SCSimConfigPopup::getDChgInput() const {
  if (ui->dchgModeComboBox->currentIndex() < 0) {
    throw std::runtime_error("Discharging input mode not selected");
  }
  return vrfb::shuntcur::ElecInput {
      static_cast<vrfb::shuntcur::ElecInput::Mode>(ui->dchgModeComboBox->currentIndex()),
      ui->dchgInputMagField->value()
  };
}









/*
********************************************************************************
**        Slots
********************************************************************************
*/


void SCSimConfigPopup::on_chgModeComboBox_currentIndexChanged(int index) {
  switch (static_cast<vrfb::shuntcur::ElecInput::Mode>(index)) {
    case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
      ui->chgInputMagUnitLabel->setText("V");
      break;
    case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
      ui->chgInputMagUnitLabel->setText("A");
      break;
    default:
      ui->chgInputMagUnitLabel->setText("");
      break;
  }
}


void SCSimConfigPopup::on_dchgModeComboBox_currentIndexChanged(int index) {
  switch (static_cast<vrfb::shuntcur::ElecInput::Mode>(index)) {
    case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
      ui->dchgInputMagUnitLabel->setText("V");
      break;
    case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
      ui->dchgInputMagUnitLabel->setText("A");
      break;
    default:
      ui->dchgInputMagUnitLabel->setText("");
      break;
  }
}


void SCSimConfigPopup::on_begSOCField_valueChanged(double value) {
  if (ui->endSOCField->value() < value) {
    ui->endSOCField->setValue(value);
  }
}


void SCSimConfigPopup::on_endSOCField_valueChanged(double value) {
  if (ui->begSOCField->value() > value) {
    ui->begSOCField->setValue(value);
  }
}
