#pragma once


#include <vector>

#include <QDialog>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"
#include "view/graphplotformtitled.h"
#include "view/mainwindow.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCResultView_PCC;
}
QT_END_NAMESPACE


class SCResultView_PCC : public MainWindow::SCDataView {
  Q_OBJECT

  using SeriesGetter = const std::vector<double>& (vrfb::shuntcur::pcc::PCCReport::*)() const;

  enum class IndexingType {
    itStack, itConn
  };

  struct PlotFormData {
    std::string name;
    IndexingType it;
    SeriesGetter yseriesGetter;
    GraphPlotFormTitled* plot;
  };

  public:
    SCResultView_PCC(QWidget*, const std::string&);
    ~SCResultView_PCC();

    void plotGraphs(const vrfb::shuntcur::pcc::PCCReport&);
    bool exportImages(logger::Logger& l) override;


  signals:
    void exportRequested(SCResultView_PCC*);


  private:
    void deleteSelf(int) {delete this;}
    void addPlot(const std::string& name, IndexingType it, SeriesGetter getter);

    Ui::SCResultView_PCC* ui;
    std::string prefix;
    std::vector<PlotFormData> formDatas;


  private slots:
    void on_exportBtn_clicked();
};
