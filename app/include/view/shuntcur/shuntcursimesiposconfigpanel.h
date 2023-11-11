#pragma once

#include <QWidget>

#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "vrfblib/vrfblib.hpp"




QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimESIPOSConfigPanel;
}
QT_END_NAMESPACE




class SCSimESIPOSConfigPanel : public SCSimConfigPopup::ConfigPanel {
  Q_OBJECT


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimESIPOSConfigPanel(QWidget* parent = nullptr);
    ~SCSimESIPOSConfigPanel();




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfb::shuntcur::ShuntCalc* getCalc() const override;




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimESIPOSConfigPanel* ui;
};
