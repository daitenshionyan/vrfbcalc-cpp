#pragma once

#include <QWidget>

#include "view/shuntcur/shuntcurconfigpopup.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCConfigFormStack;
}
QT_END_NAMESPACE


class SCConfigFormStack : public SCConfigPopup::ConfigForm {
  Q_OBJECT

  public:
    SCConfigFormStack(QWidget* parent);
    ~SCConfigFormStack();

    vrfb::shuntcur::ParamGenerator* getGenerator() override;


  private:
    Ui::SCConfigFormStack* ui;
};
