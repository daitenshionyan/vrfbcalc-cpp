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
    struct Series {
      std::string name;
      std::vector<double> xs;
      std::vector<double> ys;
    };


  public:
    GraphPlotForm(QWidget* parent);
    ~GraphPlotForm();

    void setupPlot(
        const std::vector<vrfbdriver::PerformanceEntry_CE>&,
        const std::string&, const std::string&);
    void setupPlot(const std::vector<Series>& slist,
        const std::string& name_x, std::string name_y);
    void setupPlot(
        const std::vector<double>& series_x,
        const std::vector<double>& series_y,
        const std::string& name_x, const std::string& name_y);
    bool savePng(const QString&);


  protected:
    void setupXFields(double min, double max);
    void setupYFields(double min, double max);


  private:
    class SignalHandler;


  private: // :::: private data members ::::::::::::::::::::::::::::::::::::::::
    Ui::GraphPlotForm* ui;
    SignalHandler* handler;
};
