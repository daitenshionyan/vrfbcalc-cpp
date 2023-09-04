#include "view/celleffdataentryform.h"
#include "./ui_celleffdataentryform.h"

#include <QFileDialog>


CEDataEntryForm::CEDataEntryForm(QWidget* parent)
      : QFrame(parent), ui(new Ui::CEDataEntryForm) {
  ui->setupUi(this);
}


CEDataEntryForm::~CEDataEntryForm() {
  delete ui;
}


vrfbdriver::DataEntry_CE CEDataEntryForm::getEntry() const {
  vrfb::Config_CE cfg = (ui->lfpRadioBtn->isChecked()) ?
        vrfbdriver::loadConfig_CE(ui->presetPathField->text().toStdString())
      : vrfb::Config_CE{
        ui->timeHdrField->text().toStdString(),
        ui->typeHdrField->text().toStdString(),
        ui->c_capHdrField->text().toStdString(),
        ui->d_capHdrField->text().toStdString(),
        ui->c_energyHdrField->text().toStdString(),
        ui->d_energyHdrField->text().toStdString(),
        { ui->c_typeNameField->text().toStdString() },
        { ui->d_typeNameField->text().toStdString() }
      };
  return {
    ui->pathField->text().toStdString(),
    cfg
  };
}


void CEDataEntryForm::on_browseBtn_clicked() {
  QString path = QFileDialog::getOpenFileName(this, "Open file",
      QString(), "CSV files (*.csv)");
  ui->pathField->setText(path);
}


void CEDataEntryForm::on_presetBrowseBtn_clicked() {
  QString path = QFileDialog::getOpenFileName(this, "Open file",
      QString(), "JSON files (*.json)");
  ui->presetPathField->setText(path);
}


void CEDataEntryForm::on_delBtn_clicked() {
  emit handleDelete(this);
}
