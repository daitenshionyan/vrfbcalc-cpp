#pragma once

#include <QDialog>


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


  private:
    Ui::SCConfigPopup* ui;
};
