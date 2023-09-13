#include "view/celleffdataentryform.h"
#include "./ui_celleffdataentryform.h"

#include <filesystem>
#include <sstream>

#include <QFileDialog>

#include "view/celleffloadpresetconfigform.h"
#include "view/celleffmanualconfigform.h"


CEDataEntryForm::CEDataEntryForm(QWidget* parent)
      : QFrame(parent), ui(new Ui::CEDataEntryForm) {
  ui->setupUi(this);
  ui->presetCfgBtn->setDisabled(true);
  cfgForm = new CELoadPresetConfigForm(this);
  ui->cfgFormArea->layout()->addWidget(cfgForm);

  connect(ui->manualCfgBtn, &QPushButton::clicked,
      this, &CEDataEntryForm::changeToManual);
  connect(ui->presetCfgBtn, &QPushButton::clicked,
      this, &CEDataEntryForm::changeToPreset);
}


CEDataEntryForm::~CEDataEntryForm() {
  delete ui;
  delete cfgForm;
}


vrfbdriver::DataEntry_CE CEDataEntryForm::getEntry() const {
  return {
    ui->pathField->text().toStdString(),
    ui->sheetTitleField->text().toStdString(),
    cfgForm->getConfig()
  };
}


void CEDataEntryForm::changeToManual() {
  ui->cfgFormArea->layout()->removeWidget(cfgForm);
  delete cfgForm;
  cfgForm = new CEManualConfigForm(this);
  ui->cfgFormArea->layout()->addWidget(cfgForm);
  ui->manualCfgBtn->setDisabled(true);
  ui->presetCfgBtn->setDisabled(false);
}


void CEDataEntryForm::changeToPreset() {
  ui->cfgFormArea->layout()->removeWidget(cfgForm);
  delete cfgForm;
  cfgForm = new CELoadPresetConfigForm(this);
  ui->cfgFormArea->layout()->addWidget(cfgForm);
  ui->manualCfgBtn->setDisabled(false);
  ui->presetCfgBtn->setDisabled(true);
}


void CEDataEntryForm::on_browseBtn_clicked() {
  QString path = QFileDialog::getOpenFileName(this, "Open file",
      QString(), "Data files (*.csv *.xlsx)");
  ui->pathField->setText(path);
}


void CEDataEntryForm::on_delBtn_clicked() {
  emit formDeleted(this);
}
