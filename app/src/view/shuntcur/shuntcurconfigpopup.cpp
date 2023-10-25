#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"

#include <stdexcept>


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
  ui->arrComboBox->insertItem(1, "PCC FB");

  ui->inputComboBox->insertItem(1, "CV Chg");
  ui->inputComboBox->insertItem(2, "CC Chg");
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
  if (ui->arrComboBox->currentIndex() < 0) {
    throw std::runtime_error("Arrange not set");
  }

  vrfbdriver::SCArrangement arr = static_cast<vrfbdriver::SCArrangement>(ui->arrComboBox->currentIndex());
  vrfb::shuntcur::ShuntCalc* calc = nullptr;
  switch (arr) {
    case vrfbdriver::SCArrangement::scaPCCFB:
      calc = new vrfb::shuntcur::pcc::PCCCalc(
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
                  ui->socField->value() / 100,
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
  return vrfbdriver::ShuntJob(
      ui->nameField->text().toStdString(),
      calc,
      getElecInput(),
      arr);
}


vrfb::shuntcur::ElecInput SCConfigPopup::getElecInput() const {
  return {
      static_cast<vrfb::shuntcur::ElecInput::Mode>(ui->inputComboBox->currentIndex()),
      ui->inputField->value()};
}


void SCConfigPopup::on_inputComboBox_currentIndexChanged(int index) {
  switch (static_cast<vrfb::shuntcur::ElecInput::Mode>(index)) {
    case vrfb::shuntcur::ElecInput::Mode::mConstCurr:
      ui->inputUnitLabel->setText("A");
      break;
    case vrfb::shuntcur::ElecInput::Mode::mConstVolt:
      ui->inputUnitLabel->setText("V");
      break;
  }
}
