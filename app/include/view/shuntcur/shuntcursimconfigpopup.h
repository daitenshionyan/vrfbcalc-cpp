#pragma once

#include <QDialog>

#include "driver/vrfbdriver.hpp"




QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimConfigPopup;
}
QT_END_NAMESPACE




class SCSimConfigPopup : public QDialog {
  Q_OBJECT


  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class ConfigPanel : public QWidget {
      public: // ~~~~ constructor / assignment / destructor ~~~~
        ConfigPanel(QWidget* parent=nullptr) : QWidget(parent) {}

        ConfigPanel() = delete;
        ConfigPanel(const ConfigPanel&) = delete;
        ConfigPanel(ConfigPanel&&) = delete;

        ConfigPanel& operator=(const ConfigPanel&) = delete;
        ConfigPanel& operator=(ConfigPanel&&) = delete;

        virtual ~ConfigPanel() = default;


      public: // ~~~~ functions ~~~~
        virtual vrfb::shuntcur::ShuntCalc* getCalc() const = 0;
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimConfigPopup(QWidget* parent = nullptr);
    ~SCSimConfigPopup();




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfbdriver::shuntcur::ShuntSimJob getJob();




  private slots:
    void on_arrComboBox_currentIndexChanged(int);
    void on_chgModeComboBox_currentIndexChanged(int);
    void on_dchgModeComboBox_currentIndexChanged(int);
    void on_begSOCField_valueChanged(double);
    void on_endSOCField_valueChanged(double);




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimConfigPopup* ui;
    ConfigPanel* cfgPanel = nullptr;
};
