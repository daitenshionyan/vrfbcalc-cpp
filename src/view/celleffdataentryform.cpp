#include "view/celleffdataentryform.h"
#include "./ui_celleffdataentryform.h"

#include <filesystem>
#include <sstream>

#include <QFileDialog>


CEDataEntryForm::CEDataEntryForm(QWidget* parent)
      : QFrame(parent), ui(new Ui::CEDataEntryForm) {
  ui->setupUi(this);
  connect(
      ui->c_capHdrField, &QLineEdit::textChanged,
      this, &CEDataEntryForm::on_c_capField_changed);
  connect(
      ui->c_energyHdrField, &QLineEdit::textChanged,
      this, &CEDataEntryForm::on_c_energyField_changed);
  ui->d_capRBtn->setChecked(true);
  ui->d_energyRBtn->setChecked(true);
}


CEDataEntryForm::~CEDataEntryForm() {
  delete ui;
}


vrfbdriver::DataEntry_CE CEDataEntryForm::getEntry() const {
  vrfb::Config_CE cfg = (ui->lfpRadioBtn->isChecked()) ?
        vrfbdriver::loadConfig_CE(ui->presetPathField->text().toStdString())
      : getConfig();
  return {
    ui->pathField->text().toStdString(),
    ui->sheetTitleField->text().toStdString(),
    cfg
  };
}


vrfb::Config_CE CEDataEntryForm::getConfig() const {
  vrfb::Config_CE cfg {};
  cfg.t_time_h = ui->timeHdrField->text().toStdString();
  cfg.type_h = ui->typeHdrField->text().toStdString();
  cfg.c_capacity_h = ui->c_capHdrField->text().toStdString();
  cfg.d_capacity_h = ui->d_capHdrField->text().toStdString();
  cfg.c_energy_h = ui->c_energyHdrField->text().toStdString();
  cfg.d_energy_h = ui->d_energyHdrField->text().toStdString();
  // read charging type names
  std::vector<std::string> c_names {};
  std::stringstream cn_ss{ui->c_typeNameField->text().toStdString()};
  vrfb::readLine_CSV(cn_ss, c_names);
  cfg.c_type_names.insert(c_names.begin(), c_names.end());
  // read discharging type names
  std::vector<std::string> d_names {};
  std::stringstream dn_ss{ui->d_typeNameField->text().toStdString()};
  vrfb::readLine_CSV(dn_ss, d_names);
  cfg.d_type_names.insert(d_names.begin(), d_names.end());
  return cfg;
}


void CEDataEntryForm::on_browseBtn_clicked() {
  QString path = QFileDialog::getOpenFileName(this, "Open file",
      QString(), "Data files (*.csv *.xlsx)");
  ui->pathField->setText(path);
}


void CEDataEntryForm::on_presetBrowseBtn_clicked() {
  QString preset_path{};
  std::filesystem::path output_path {"presets"};
  if (std::filesystem::exists(output_path) && std::filesystem::is_directory(output_path)) {
    preset_path = "presets";
  }
  QString path = QFileDialog::getOpenFileName(this, "Open file",
      preset_path, "JSON files (*.json)");
  ui->presetPathField->setText(path);
}


void CEDataEntryForm::on_d_capRBtn_toggled(bool t) {
  if (t) {
    ui->d_capHdrField->setText(ui->c_capHdrField->text());
  }
  ui->d_capHdrField->setDisabled(t);
}


void CEDataEntryForm::on_d_energyRBtn_toggled(bool t) {
  if (t) {
    ui->d_energyHdrField->setText(ui->c_energyHdrField->text());
  }
  ui->d_energyHdrField->setDisabled(t);
}


void CEDataEntryForm::on_delBtn_clicked() {
  emit handleDelete(this);
}


void CEDataEntryForm::on_c_capField_changed() {
  if (ui->d_capRBtn->isChecked()) {
    ui->d_capHdrField->setText(ui->c_capHdrField->text());
  }
}


void CEDataEntryForm::on_c_energyField_changed() {
  if (ui->d_energyRBtn->isChecked()) {
    ui->d_energyHdrField->setText(ui->c_energyHdrField->text());
  }
}
