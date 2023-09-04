#pragma once

#include <functional>
#include <vector>

#include <QFrame>

#include "vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEDataEntryForm;
}
QT_END_NAMESPACE


class CEDataEntryForm : public QFrame {
  Q_OBJECT

  public:
    CEDataEntryForm(QWidget* parent);
    ~CEDataEntryForm();

    vrfbdriver::DataEntry_CE getEntry() const;


  signals:
    void handleDelete(CEDataEntryForm*);


  private slots:
    void on_browseBtn_clicked();
    void on_presetBrowseBtn_clicked();
    void on_delBtn_clicked();


  private:
    Ui::CEDataEntryForm* ui;
};
