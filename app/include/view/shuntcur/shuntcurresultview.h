#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"
#include "view/graphplotform.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCResultView;
}
QT_END_NAMESPACE


class SCResultView : public QDialog {
  Q_OBJECT

  enum class IndexingType {
    itStack, itConn
  };

  struct PlotFormData {
    std::string name;
    IndexingType it;
    std::vector<double> (vrfb::shuntcur::scl::SCLReport::* yseriesGetter)() const;
    GraphPlotForm* plot;
  };

  public:
    SCResultView(QWidget*, const std::string&);
    ~SCResultView();

    void plotGraphs(const vrfb::shuntcur::scl::SCLReport&);
    bool exportImages(logger::Logger& l);


  signals:
    void exportRequested(SCResultView*);


  private:
    void deleteSelf(int) {delete this;}

    Ui::SCResultView* ui;
    std::string prefix;
    std::vector<PlotFormData> formDatas;


  private slots:
    void on_exportBtn_clicked();
};
