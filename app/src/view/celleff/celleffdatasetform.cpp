#include "view/celleff/celleffdatasetform.h"
#include "./ui_celleffdatasetform.h"

#include <algorithm>


CEDataSetForm::CEDataSetForm(QWidget* parent)
      : QWidget(parent), ui(new Ui::CEDataSetForm) {
  ui->setupUi(this);
}


CEDataSetForm::~CEDataSetForm() {
  for (CEDataEntryForm* f : forms) {
    delete f;
  }
  delete ui;
}


std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>> CEDataSetForm::getSetSupplier() const {
  return {
    ui->nameField->text().toStdString(),
    [this](){
      std::vector<vrfbdriver::DataEntry_CE> dataEntries {};
      for (const CEDataEntryForm* f : forms) {
        dataEntries.push_back(f->getEntry());
      }
      return vrfbdriver::DataSet_CE{
        ui->areaField->value(),
        dataEntries
      };
    }
  };
}


void CEDataSetForm::on_delBtn_clicked() {
  emit formDeleted(this);
}


void CEDataSetForm::on_addEntryBtn_clicked() {
  forms.push_back(new CEDataEntryForm(this));
  ui->entriesArea->layout()->addWidget(forms[forms.size()-1]);
  connect(forms[forms.size()-1], &CEDataEntryForm::formDeleted,
      this, &CEDataSetForm::delForm);
  forms[forms.size()-1]->show();
}


void CEDataSetForm::delForm(CEDataEntryForm* f) {
  ui->entriesArea->layout()->removeWidget(f);
  forms.erase(std::remove(forms.begin(), forms.end(), f), forms.end());
  delete f;
}
