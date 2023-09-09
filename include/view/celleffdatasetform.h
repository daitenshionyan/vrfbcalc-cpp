#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <QWidget>

#include "view/celleffdataentryform.h"
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
    void handleDelete(CEDataSetForm*);


  private slots:
    void on_delBtn_clicked();
    void on_addEntryBtn_clicked();


  private:
    void on_entry_del(CEDataEntryForm*);

    Ui::CEDataSetForm* ui;
    std::vector<CEDataEntryForm*> forms;
};
