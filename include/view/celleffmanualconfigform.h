#pragma once


#include <QWidget>

#include "view/celleffdataentryform.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEManualConfigForm;
}
QT_END_NAMESPACE


class CEManualConfigForm : public CEDataEntryForm::ConfigForm {
  public:
    CEManualConfigForm(QWidget* parent);
    ~CEManualConfigForm();

    vrfb::Config_CE getConfig() const override;


  private:
    Ui::CEManualConfigForm* ui;


  private slots:
    void on_chgCapHdrField_textChanged(const QString&);
    void on_chgEnergyHdrField_textChanged(const QString&);
    void on_dchgCapCopyRBtn_toggled(bool);
    void on_dchgEnergyCopyRBtn_toggled(bool);
};
