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

  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimConfigPopup(QWidget* parent = nullptr);
    ~SCSimConfigPopup();


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfbdriver::shuntcur::ShuntSimJob getJob();


  private: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vrfb::shuntcur::ShuntCalc* getCalc() const;
    vrfb::shuntcur::ElecInput getChgInput() const;
    vrfb::shuntcur::ElecInput getDChgInput() const;


  private slots:
    void on_chgModeComboBox_currentIndexChanged(int);
    void on_dchgModeComboBox_currentIndexChanged(int);
    void on_begSOCField_valueChanged(double);
    void on_endSOCField_valueChanged(double);


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimConfigPopup* ui;
};
