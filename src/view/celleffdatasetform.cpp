#include "view/celleffdatasetform.h"
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


std::pair<std::string, vrfbdriver::DataSet_CE> CEDataSetForm::getDataSet() const {
  std::vector<vrfbdriver::DataEntry_CE> dataEntries {};
  for (const CEDataEntryForm* e : forms) {
    dataEntries.push_back(e->getEntry());
  }
  return {
    ui->nameField->text().toStdString(),
    {
      ui->areaField->text().toDouble(),
      dataEntries
    }
  };
}


void CEDataSetForm::on_delBtn_clicked() {
  emit handleDelete(this);
}


void CEDataSetForm::on_addEntryBtn_clicked() {
  forms.push_back(new CEDataEntryForm(this));
  ui->entriesArea->layout()->addWidget(forms[forms.size()-1]);
  connect(forms[forms.size()-1], &CEDataEntryForm::handleDelete,
      this, &CEDataSetForm::on_entry_del);
  forms[forms.size()-1]->show();
}


void CEDataSetForm::on_entry_del(CEDataEntryForm* f) {
  ui->entriesArea->layout()->removeWidget(f);
  forms.erase(std::remove(forms.begin(), forms.end(), f), forms.end());
  delete f;
}
