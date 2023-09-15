#pragma once

#include <string>
#include <vector>

#include <QWidget>

#include "table.hpp"


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

    void initialiseData(const std::vector<vrfb::Table>&,
        const std::string&, const std::string&);
    bool savePng(const QString&);


  private:
    Ui::GraphPlotForm* ui;


  private slots:
    void on_xAxisLowerField_valueChanged(double);
    void on_xAxisUpperField_valueChanged(double);
    void on_yAxisLowerField_valueChanged(double);
    void on_yAxisUpperField_valueChanged(double);
};
