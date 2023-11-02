#pragma once

#include <QDialog>

#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCConfigPopup;
}
QT_END_NAMESPACE


class SCConfigPopup : public QDialog {
  Q_OBJECT


  public:
    SCConfigPopup(QWidget* parent);
    ~SCConfigPopup();

    vrfbdriver::shuntcur::ShuntJob getJob();


  public slots:
    void on_inputComboBox_currentIndexChanged(int);


  private:
    vrfb::shuntcur::ElecInput getElecInput() const;


  private:
    Ui::SCConfigPopup* ui;
};
