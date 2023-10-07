#pragma once

#include <string>
#include <vector>

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
  class GraphPlotFormTitled;
}
QT_END_NAMESPACE


class GraphPlotFormTitled : public QWidget {
  Q_OBJECT

  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    GraphPlotFormTitled(const std::string& title, QWidget* parent = nullptr);
    ~GraphPlotFormTitled();


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setupPlot(
        const std::vector<double>& series_x,
        const std::vector<double>& series_y,
        const std::string& name_x, const std::string& name_y);
    bool savePng(const QString&);


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::GraphPlotFormTitled* ui;
};
