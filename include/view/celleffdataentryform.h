#pragma once

#include <unordered_set>
#include <vector>

#include <QFrame>

#include "driver/vrfbdriver.hpp"


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
    void formDeleted(CEDataEntryForm*);


  private:
    void syncCapField();
    void synchEnergyField();

    vrfb::Config_CE getConfig() const;

    Ui::CEDataEntryForm* ui;


  private slots:
    void on_browseBtn_clicked();
    void on_presetBrowseBtn_clicked();
    void on_d_capRBtn_toggled(bool);
    void on_d_energyRBtn_toggled(bool);
    void on_delBtn_clicked();
};
