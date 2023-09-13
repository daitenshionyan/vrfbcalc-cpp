#include "view/celleffloadpresetconfigform.h"
#include "./ui_celleffloadpresetconfigform.h"

#include <QFileDialog>

#include "driver/vrfbdriver_io.hpp"


CELoadPresetConfigForm::CELoadPresetConfigForm(QWidget* parent)
    : CEDataEntryForm::ConfigForm(parent),
      ui(new Ui::CELoadPresetConfigForm) {
  ui->setupUi(this);
}


CELoadPresetConfigForm::~CELoadPresetConfigForm() {
  delete ui;
}


vrfb::Config_CE CELoadPresetConfigForm::getConfig() const {
  auto p = std::filesystem::u8path<std::string>(ui->pathField->text().toStdString());
  return vrfbdriver::io::loadConfig_CE(p);
}


void CELoadPresetConfigForm::on_browseBtn_clicked() {
  QString path = QFileDialog::getOpenFileName(this, "Open preset",
      QString(), "Preset files (*.json)");
  ui->pathField->setText(path);
}
