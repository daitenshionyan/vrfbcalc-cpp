#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "./ui_shuntcursimconfigpopup.h"

#include <stdexcept>

#include "view/shuntcur/shuntcursimpccconfigpanel.h"
#include "view/shuntcur/shuntcursimesiposconfigpanel.h"




namespace {


vrfb::shuntcur::ElecInput getChgInput(const Ui::SCSimConfigPopup* ui) {
  if (ui->chgModeComboBox->currentIndex() < 0) {
    throw std::runtime_error("Charging input mode not selected");
  }
  return vrfb::shuntcur::ElecInput {
      static_cast<vrfb::shuntcur::ElecInput::Mode>(ui->chgModeComboBox->currentIndex()),
      ui->chgInputMagField->value()
  };
}


vrfb::shuntcur::ElecInput getDChgInput(const Ui::SCSimConfigPopup* ui) {
  if (ui->dchgModeComboBox->currentIndex() < 0) {
    throw std::runtime_error("Discharging input mode not selected");
  }
  return vrfb::shuntcur::ElecInput {
      static_cast<vrfb::shuntcur::ElecInput::Mode>(ui->dchgModeComboBox->currentIndex()),
      ui->dchgInputMagField->value()
  };
}


}
































SCSimConfigPopup::SCSimConfigPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::SCSimConfigPopup) {
  ui->setupUi(this);
  ui->arrComboBox->insertItem(1, "PCC FB");
  ui->arrComboBox->insertItem(2, "ESIPOS");
  // add values to elec input mode combo box
  ui->chgModeComboBox->insertItem(1, "Const Volt");
  ui->chgModeComboBox->insertItem(2, "Const Curr");
  ui->chgModeComboBox->insertItem(3, "Const Powr");
  ui->dchgModeComboBox->insertItem(1, "Const Volt");
  ui->dchgModeComboBox->insertItem(2, "Const Curr");
  ui->dchgModeComboBox->insertItem(3, "Const Powr");
}


SCSimConfigPopup::~SCSimConfigPopup() {
  delete ui;
}


vrfbdriver::shuntcur::ShuntSimJob SCSimConfigPopup::getJob() {
  if (ui->nameField->text().isEmpty()) {
    throw std::runtime_error("Blank name");
  }
  if (ui->arrComboBox->currentIndex() < 0) {
    throw std::runtime_error("Arrange not set");
  }
  if (cfgPanel == nullptr) {
    throw std::runtime_error("Unsupported arrangement");
  }

  return vrfbdriver::shuntcur::ShuntSimJob {
    ui->nameField->text().toStdString(),
    cfgPanel->getCalc(),
    ui->volField->value(),
    ui->concField->value(),
    getChgInput(ui),
    getDChgInput(ui),
    new vrfbdriver::shuntcur::EndPointSOC<vrfbdriver::shuntcur::InputEndPoint::LimitType::ltUpper>(
        ui->endSOCField->value() / 100),
    new vrfbdriver::shuntcur::EndPointSOC<vrfbdriver::shuntcur::InputEndPoint::LimitType::ltLower>(
        ui->begSOCField->value() / 100),
    ui->begSOCField->value() / 100,
    ui->endSOCField->value() / 100,
    static_cast<vrfbdriver::shuntcur::SCArrangement>(ui->arrComboBox->currentIndex())
  };
}









/*
********************************************************************************
**        Slots
********************************************************************************
*/


void SCSimConfigPopup::on_arrComboBox_currentIndexChanged(int index) {
  if (cfgPanel != nullptr) {
    ui->sysConfigArea->layout()->removeWidget(cfgPanel);
    delete cfgPanel;
    cfgPanel = nullptr;
  }
  switch (static_cast<vrfbdriver::shuntcur::SCArrangement>(index)) {
    case vrfbdriver::shuntcur::SCArrangement::scaPCCFB:
      cfgPanel = new SCSimPCCConfigPanel(this);
      ui->sysConfigArea->layout()->addWidget(cfgPanel);
      break;
    case vrfbdriver::shuntcur::SCArrangement::scaESIPOS:
      cfgPanel = new SCSimESIPOSConfigPanel(this);
      ui->sysConfigArea->layout()->addWidget(cfgPanel);
      break;
    default:
      ui->sysConfigArea->layout()->removeWidget(cfgPanel);
      delete cfgPanel;
      cfgPanel = nullptr;
      break;
  }
}


void SCSimConfigPopup::on_chgModeComboBox_currentIndexChanged(int index) {
  switch (static_cast<vrfb::shuntcur::ElecInput::Mode>(index)) {
    case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
      ui->chgInputMagUnitLabel->setText("V");
      break;
    case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
      ui->chgInputMagUnitLabel->setText("A");
      break;
    case vrfb::shuntcur::ElecInput::Mode::mConstPowr:
      ui->chgInputMagUnitLabel->setText("W");
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
    case vrfb::shuntcur::ElecInput::Mode::mConstPowr:
      ui->dchgInputMagUnitLabel->setText("W");
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
