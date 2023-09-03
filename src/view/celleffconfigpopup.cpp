#include "view/celleffconfigpopup.h"
#include "./ui_celleffconfigpopup.h"

#include <algorithm>

#include <QFileDialog>


CEConfigPopup::CEConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::CEConfigPopup) {
  ui->setupUi(this);
}


CEConfigPopup::~CEConfigPopup() {
  for (CEDataSetForm* e : forms) {
    delete e;
  }
  delete ui;
}


std::unordered_map<std::string, vrfbdriver::DataSet_CE> CEConfigPopup::getDataSets() const {
  std::unordered_map<std::string, vrfbdriver::DataSet_CE> res {};
  for (const CEDataSetForm* f : forms) {
    res.insert(f->getDataSet());
  }
  return res;
}


void CEConfigPopup::on_addBtn_clicked() {
  forms.push_back(new CEDataSetForm(this));
  ui->entriesArea->layout()->addWidget(forms[forms.size()-1]);
  connect(forms[forms.size()-1], &CEDataSetForm::handleDelete,
      this, &CEConfigPopup::on_entry_del);
  forms[forms.size()-1]->show();
}


void CEConfigPopup::on_entry_del(CEDataSetForm* f) {
  ui->entriesArea->layout()->removeWidget(f);
  forms.erase(std::remove(forms.begin(), forms.end(), f), forms.end());
  delete f;
}
