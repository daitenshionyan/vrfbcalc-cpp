#pragma once

#include <QWidget>

#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "vrfblib/vrfblib.hpp"




QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimPCCConfigPanel;
}
QT_END_NAMESPACE




class SCSimPCCConfigPanel : public SCSimConfigPopup::ConfigPanel {
  Q_OBJECT


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimPCCConfigPanel(QWidget* parent = nullptr);
    ~SCSimPCCConfigPanel();




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfb::shuntcur::ShuntCalc* getCalc() const override;




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimPCCConfigPanel* ui;
};
