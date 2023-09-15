#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEResultView;
}
QT_END_NAMESPACE


class CEResultView : public QDialog {
  Q_OBJECT

  public:
    CEResultView(QWidget*);
    ~CEResultView();

    void plotGraphs(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    void exportPng();


  private:
    void deleteSelf(int) {delete this;}

    Ui::CEResultView* ui;


  private slots:
    void on_exportBtn_clicked();
};
