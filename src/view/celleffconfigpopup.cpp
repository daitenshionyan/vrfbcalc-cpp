#include "view/celleffconfigpopup.h"
#include "./ui_celleffconfigpopup.h"

#include <algorithm>

#include <QFileDialog>


CellEffConfigPopup::CellEffConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::CellEffConfigPopup) {
  ui->setupUi(this);
}


CellEffConfigPopup::~CellEffConfigPopup() {
  delete ui;
}


std::unordered_map<std::string, vrfbdriver::DataSet_CE> CellEffConfigPopup::getDataSets() const {
  std::unordered_map<std::string, vrfbdriver::DataSet_CE> res {};
  for (const CEDataSetForm* e : entries) {
    res.insert(e->getDataSet());
  }
  return res;
}


void CellEffConfigPopup::on_addBtn_clicked() {
  entries.push_back(new CEDataSetForm(this));
  ui->entriesArea->layout()->addWidget(entries[entries.size()-1]);
  connect(entries[entries.size()-1], &CEDataSetForm::handleDelete,
      this, &CellEffConfigPopup::on_entry_del);
  entries[entries.size()-1]->show();
}


void CellEffConfigPopup::on_entry_del(CEDataSetForm* entry) {
  ui->entriesArea->layout()->removeWidget(entry);
  entries.erase(std::remove(entries.begin(), entries.end(), entry), entries.end());
  delete entry;
}
