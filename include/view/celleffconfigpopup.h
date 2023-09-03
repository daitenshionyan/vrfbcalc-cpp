#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <QDialog>

#include "view/celleffdatasetform.h"
#include "vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CellEffConfigPopup;
}
QT_END_NAMESPACE


class CellEffConfigPopup : public QDialog {
  Q_OBJECT

 public:
  CellEffConfigPopup(QWidget* parent);
  ~CellEffConfigPopup();

  std::unordered_map<std::string, vrfbdriver::DataSet_CE> getDataSets() const;


 private slots:
  void on_addBtn_clicked();
  void on_entry_del(CEDataSetForm*);


 private:
  Ui::CellEffConfigPopup* ui;
  std::vector<CEDataSetForm*> entries;
};
