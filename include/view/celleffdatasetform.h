#pragma once

#include <string>
#include <utility>
#include <vector>

#include <QWidget>

#include "view/celleffdataentryform.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEDataSetForm;
}
QT_END_NAMESPACE


class CEDataSetForm : public QWidget {
  Q_OBJECT

 public:
  CEDataSetForm(QWidget* parent);
  ~CEDataSetForm();

  std::pair<std::string, vrfbdriver::DataSet_CE> getDataSet() const;


 signals:
  void handleDelete(CEDataSetForm*);


 private slots:
  void on_delBtn_clicked();
  void on_addEntryBtn_clicked();
  void on_entry_del(CEDataEntryForm*);


 private:
  Ui::CEDataSetForm* ui;
  std::vector<CEDataEntryForm*> forms;
};
