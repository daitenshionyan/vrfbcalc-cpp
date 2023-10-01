#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <QWidget>

#include "view/celleff/celleffdataentryform.h"
#include "driver/vrfbdriver.hpp"


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

    std::pair<std::string, std::function<vrfbdriver::DataSet_CE()>> getSetSupplier() const;


  signals:
    void formDeleted(CEDataSetForm*);


  private:
    void delForm(CEDataEntryForm*);

    Ui::CEDataSetForm* ui;
    std::vector<CEDataEntryForm*> forms;


  private slots:
    void on_delBtn_clicked();
    void on_addEntryBtn_clicked();
};
