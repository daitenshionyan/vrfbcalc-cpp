#include "view/celleff/celleffloadpresetconfigform.h"
#include "./ui_celleffloadpresetconfigform.h"

#include <filesystem>

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
  QString openPath = "presets";
  if (!std::filesystem::exists(openPath.toStdString())) {
    openPath = QString();
  }
  QString path = QFileDialog::getOpenFileName(this, "Open preset",
      openPath, "Preset files (*.json)");
  ui->pathField->setText(path);
}
