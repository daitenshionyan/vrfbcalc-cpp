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
    class ConfigForm : public QWidget {
      public:
        ConfigForm(QWidget* parent) : QWidget(parent) {};
        virtual ~ConfigForm() = default;
        virtual vrfb::Config_CE getConfig() const = 0;
    };


    CEDataEntryForm(QWidget* parent);
    ~CEDataEntryForm();

    vrfbdriver::DataEntry_CE getEntry() const;


  signals:
    void formDeleted(CEDataEntryForm*);


  private:
    void changeToManual();
    void changeToPreset();

    Ui::CEDataEntryForm* ui;
    ConfigForm* cfgForm;


  private slots:
    void on_browseBtn_clicked();
    void on_delBtn_clicked();
};
