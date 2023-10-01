#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCResultView;
}
QT_END_NAMESPACE


class SCResultView : public QDialog {
  Q_OBJECT

  public:
    SCResultView(QWidget*);
    ~SCResultView();

    void plotGraphs(const vrfb::shuntcur::ShuntPerf&);


  private:
    void deleteSelf(int) {delete this;}

    Ui::SCResultView* ui;
};
