#include "view/celleffmanualconfigform.h"
#include "./ui_celleffmanualconfigform.h"

#include <sstream>
#include <string>
#include <vector>

#include "driver/vrfbdriver_io.hpp"


CEManualConfigForm::CEManualConfigForm(QWidget* parent)
    : CEDataEntryForm::ConfigForm(parent),
      ui(new Ui::CEManualConfigForm) {
  ui->setupUi(this);
}


CEManualConfigForm::~CEManualConfigForm() {
  delete ui;
}


vrfb::Config_CE CEManualConfigForm::getConfig() const {
  vrfb::Config_CE cfg {};
  cfg.t_time_h = ui->timeHdrField->text().toStdString();
  cfg.type_h = ui->typeHdrField->text().toStdString();
  cfg.c_capacity_h = ui->chgCapHdrField->text().toStdString();
  cfg.d_capacity_h = ui->dchgCapHdrField->text().toStdString();
  cfg.c_energy_h = ui->chgCapHdrField->text().toStdString();
  cfg.d_energy_h = ui->dchgCapHdrField->text().toStdString();
  std::vector<std::string> c_names {};
  std::stringstream cn_ss{ui->chgTypeNamesField->text().toStdString()};
  vrfbdriver::io::readLine_CSV(cn_ss, c_names);
  cfg.c_type_names.insert(c_names.begin(), c_names.end());
  std::vector<std::string> d_names {};
  std::stringstream dn_ss{ui->dchgTypeNamesField->text().toStdString()};
  vrfbdriver::io::readLine_CSV(dn_ss, d_names);
  cfg.d_type_names.insert(d_names.begin(), d_names.end());
  return cfg;
}


void CEManualConfigForm::on_chgCapHdrField_textChanged(const QString& text) {
  if (ui->dchgCapCopyRBtn->isChecked()) {
    ui->dchgCapHdrField->setText(text);
  }
}


void CEManualConfigForm::on_chgEnergyHdrField_textChanged(const QString& text) {
  if (ui->dchgEnergyCopyRBtn->isChecked()) {
    ui->dchgEnergyHdrField->setText(text);
  }
}


void CEManualConfigForm::on_dchgCapCopyRBtn_toggled(bool is_checked) {
  ui->dchgCapHdrField->setDisabled(is_checked);
  if (is_checked) {
    ui->dchgCapHdrField->setText(ui->chgCapHdrField->text());
  }
}


void CEManualConfigForm::on_dchgEnergyCopyRBtn_toggled(bool is_checked) {
  ui->dchgEnergyHdrField->setDisabled(is_checked);
  if (is_checked) {
    ui->dchgEnergyHdrField->setText(ui->chgEnergyHdrField->text());
  }
}
