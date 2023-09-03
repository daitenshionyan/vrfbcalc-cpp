#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <QDialog>

#include "view/celleffdatasetform.h"
#include "vrfbdriver.hpp"


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

  std::unordered_map<std::string, vrfbdriver::DataSet_CE> getDataSets() const;


 private slots:
  void on_addBtn_clicked();
  void on_entry_del(CEDataSetForm*);


 private:
  Ui::CEConfigPopup* ui;
  std::vector<CEDataSetForm*> forms;
};
