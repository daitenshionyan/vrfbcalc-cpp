#pragma once

#include <string>
#include <vector>

#include <QWidget>

#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class GraphPlotForm;
}
QT_END_NAMESPACE


class GraphPlotForm : public QWidget {
  Q_OBJECT

  public:
    GraphPlotForm(QWidget* parent);
    ~GraphPlotForm();

    void setupPlot(
        const std::vector<vrfbdriver::PerformanceEntry_CE>&,
        const std::string&, const std::string&);
    bool savePng(const QString&);


  protected:
    void setupXFields(double min, double max);
    void setupYFields(double min, double max);


  private:  // :::: private slots ::::::::::::::::::::::::::::::::::::::::::::::
    void setXAxisLowerPlot(double);
    void setXAxisUpperPlot(double);
    void setYAxisLowerPlot(double);
    void setYAxisUpperPlot(double);


  private: // :::: private data members ::::::::::::::::::::::::::::::::::::::::
    Ui::GraphPlotForm* ui;
    double xDiffThreshold;
    double yDiffThreshold;
};
