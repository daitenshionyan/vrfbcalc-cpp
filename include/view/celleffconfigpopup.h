#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <QDialog>

#include "view/celleffdatasetform.h"
#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEConfigPopup;
}
QT_END_NAMESPACE


class CEConfigPopup : public QDialog {
  Q_OBJECT

  public:
    CEConfigPopup(QWidget* parent);
    ~CEConfigPopup();

    vrfbdriver::SetSupplierVec_CE getSetSupplierMap() const;


  private slots:
    void on_addBtn_clicked();


  private:
    void on_entry_del(CEDataSetForm*);

    Ui::CEConfigPopup* ui;
    std::vector<CEDataSetForm*> forms;
};
