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


  private:
    void updateXAxis();
    void updateYAxis();

    Ui::GraphPlotForm* ui;
};
