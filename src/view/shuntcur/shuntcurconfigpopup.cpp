#include "view/shuntcur/shuntcurconfigpopup.h"
#include "./ui_shuntcurconfigpopup.h"


SCConfigPopup::SCConfigPopup(QWidget* parent)
    : QDialog(parent), ui(new Ui::SCConfigPopup) {
  ui->setupUi(this);
}


SCConfigPopup::~SCConfigPopup() {
  delete ui;
}
