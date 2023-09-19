#pragma once


#include <QWidget>

#include "view/celleff/celleffdataentryform.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CELoadPresetConfigForm;
}
QT_END_NAMESPACE


class CELoadPresetConfigForm : public CEDataEntryForm::ConfigForm {
  Q_OBJECT

  public:
    CELoadPresetConfigForm(QWidget* parent);
    ~CELoadPresetConfigForm();

    vrfb::Config_CE getConfig() const override;


  private:
    Ui::CELoadPresetConfigForm* ui;


  private slots:
    void on_browseBtn_clicked();
};
