#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"

#include <stdexcept>


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
  ui->arrComboBox->insertItem(1, "SCL FF");
  ui->arrComboBox->insertItem(2, "SCL FB");
  ui->arrComboBox->insertItem(3, "PCC FB");
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
    case vrfbdriver::SCArrangement::scaSCLFF:
      calc = new vrfb::shuntcur::scl::SCLCalc(
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
        },
        vrfb::shuntcur::scl::SCLCalc::ConnType::ctFF);
      break;
    case vrfbdriver::SCArrangement::scaSCLFB:
      calc = new vrfb::shuntcur::scl::SCLCalc(
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
        },
        vrfb::shuntcur::scl::SCLCalc::ConnType::ctFB);
      break;
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
  }
  return vrfbdriver::ShuntJob(
      ui->nameField->text().toStdString(),
      calc,
      ui->chgVoltField->value(),
      arr);
}
