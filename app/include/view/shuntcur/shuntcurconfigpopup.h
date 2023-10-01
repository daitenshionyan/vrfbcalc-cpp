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

    vrfbdriver::ShuntJob getJob();


  private:
    Ui::SCConfigPopup* ui;
};
