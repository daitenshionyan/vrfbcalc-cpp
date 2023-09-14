#pragma once


#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEResultView;
}
QT_END_NAMESPACE


class CEResultView : public QDialog {
  Q_OBJECT

  public:
    CEResultView(QWidget*, const QStringList&);
    ~CEResultView();


  private:
    Ui::CEResultView* ui;
};
