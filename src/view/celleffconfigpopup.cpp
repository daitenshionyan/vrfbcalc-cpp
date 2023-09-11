#include "view/celleffconfigpopup.h"
#include "./ui_celleffconfigpopup.h"

#include <algorithm>

#include <QFileDialog>


CEConfigPopup::CEConfigPopup(QWidget* parent)
      : QDialog(parent), ui(new Ui::CEConfigPopup) {
  ui->setupUi(this);
}


CEConfigPopup::~CEConfigPopup() {
  for (CEDataSetForm* f : forms) {
    delete f;
  }
  delete ui;
}


vrfbdriver::SetSupplierVec_CE CEConfigPopup::getSetSupplierMap() const {
  vrfbdriver::SetSupplierVec_CE vec {};
  for (const CEDataSetForm* f : forms) {
    vec.push_back(f->getSetSupplier());
  }
  return vec;
}


void CEConfigPopup::on_addBtn_clicked() {
  forms.push_back(new CEDataSetForm(this));
  ui->entriesArea->layout()->addWidget(forms[forms.size()-1]);
  connect(forms[forms.size()-1], &CEDataSetForm::formDeleted,
      this, &CEConfigPopup::delForm);
  forms[forms.size()-1]->show();
}


void CEConfigPopup::delForm(CEDataSetForm* f) {
  ui->entriesArea->layout()->removeWidget(f);
  forms.erase(std::remove(forms.begin(), forms.end(), f), forms.end());
  delete f;
}
