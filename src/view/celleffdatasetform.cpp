#include "view/celleffdatasetform.h"
#include "./ui_celleffdatasetform.h"

#include <algorithm>


CEDataSetForm::CEDataSetForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::CEDataSetForm) {
  ui->setupUi(this);
}


CEDataSetForm::~CEDataSetForm() {
  delete ui;
}


std::pair<std::string, vrfbdriver::DataSet_CE> CEDataSetForm::getDataSet() const {
  std::vector<vrfbdriver::DataEntry_CE> dataEntries {};
  for (const CEDataEntryForm* e : entries) {
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
  entries.push_back(new CEDataEntryForm(this));
  ui->entriesArea->layout()->addWidget(entries[entries.size()-1]);
  connect(entries[entries.size()-1], &CEDataEntryForm::handleDelete,
      this, &CEDataSetForm::on_entry_del);
  entries[entries.size()-1]->show();
}


void CEDataSetForm::on_entry_del(CEDataEntryForm* entry) {
  ui->entriesArea->layout()->removeWidget(entry);
  entries.erase(std::remove(entries.begin(), entries.end(), entry), entries.end());
  delete entry;
}
