#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"

#include <utility>
#include <vector>

#include "graphplotform.h"


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
    bool exportImages(logger::Logger&);

  signals:
    void exportRequested(CEResultView*);


  private:
    void deleteSelf(int) {delete this;}

    Ui::CEResultView* ui;
    std::vector<std::pair<std::string, GraphPlotForm*>> plotForms;


  private slots:
    void on_exportBtn_clicked();
};
