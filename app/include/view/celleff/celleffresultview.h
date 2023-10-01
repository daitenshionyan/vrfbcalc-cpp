#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"

#include <string>
#include <utility>
#include <vector>

#include "view/graphplotform.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEResultView;
}
QT_END_NAMESPACE


class CEResultView : public QDialog {
  Q_OBJECT

  struct PlotFormData {
    std::string title;
    std::string name;
    std::string xHdr;
    std::string yHdr;
    GraphPlotForm* plot;
  };

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
    std::vector<PlotFormData> plotForms;


  private slots:
    void on_exportBtn_clicked();
};
