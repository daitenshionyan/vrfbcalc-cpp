#pragma once

#include <QWidget>

#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "vrfblib/vrfblib.hpp"




QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimESIPOS2ConfigPanel;
}
QT_END_NAMESPACE




class SCSimESIPOS2ConfigPanel : public SCSimConfigPopup::ConfigPanel {
  Q_OBJECT


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimESIPOS2ConfigPanel(QWidget* parent = nullptr);
    ~SCSimESIPOS2ConfigPanel();




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfb::shuntcur::ShuntCalc* getCalc() const override;




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimESIPOS2ConfigPanel* ui;
};
