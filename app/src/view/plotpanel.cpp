#include "view/plotpanel.h"
#include "./ui_plotpanel.h"

#include <cmath>
#include <stdexcept>








namespace {


constexpr double kDiffThreshold = 1e-9;


}
































/*
================================================================================
================================================================================
==
==        SignaleHandler Definition
==
================================================================================
================================================================================
*/


class PlotPanel::SignalHandler : public QObject {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SignalHandler(Ui::PlotPanel* uip) : ui(uip) {
      // dimension fields -> plot dimensions
      connect(ui->plotWidthField, &QSpinBox::valueChanged,
          this, &SignalHandler::setPlotWidth);
      connect(ui->plotHeightField, &QSpinBox::valueChanged,
          this, &SignalHandler::setPlotHeight);
      // range fields -> plot axis range
      connect(ui->xAxisLowerField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setXAxisLowerRange);
      connect(ui->xAxisUpperField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setXAxisUpperRange);
      connect(ui->yAxisLowerField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxisLowerRange);
      connect(ui->yAxisUpperField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxisUpperRange);
      connect(ui->yAxis2LowerField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxis2LowerRange);
      connect(ui->yAxis2UpperField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxis2UpperRange);
      // plot axis range -> range fields
      connect(ui->plot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
          this, &SignalHandler::setXAxisRangeFields);
      connect(ui->plot->yAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
          this, &SignalHandler::setYAxisRangeFields);
      connect(ui->plot->yAxis2, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
          this, &SignalHandler::setYAxis2RangeFields);
      // tick count field -> plot tick count
      connect(ui->xAxisTickCountField, &QSpinBox::valueChanged,
          this, &SignalHandler::setXAxisTickCount);
      connect(ui->yAxisTickCountField, &QSpinBox::valueChanged,
          this, &SignalHandler::setYAxisTickCount);
      connect(ui->yAxis2TickCountField, &QSpinBox::valueChanged,
          this, &SignalHandler::setYAxis2TickCount);
      connect(ui->xAxisForceCountCB, &QCheckBox::toggled,
          this, &SignalHandler::setXAxisForceCountPlot);
      connect(ui->yAxisForceCountCB, &QCheckBox::toggled,
          this, &SignalHandler::setYAxisForceCountPlot);
      connect(ui->yAxis2ForceCountCB, &QCheckBox::toggled,
          this, &SignalHandler::setYAxis2ForceCountPlot);
    }


    SignalHandler() = delete;
    SignalHandler(const PlotPanel::SignalHandler&) = default;
    SignalHandler(PlotPanel::SignalHandler&&) = default;

    SignalHandler& operator=(const PlotPanel::SignalHandler&) = default;
    SignalHandler& operator=(PlotPanel::SignalHandler&&) = default;

    ~SignalHandler() = default;




  private: // ~~~~ plot dimension functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setPlotWidth(int width) {
      if (ui->plot->width() == width) {
        return;
      }
      ui->plot->setMinimumWidth(width);
      ui->plot->setMaximumWidth(width);
      ui->plot->replot();
    }


    void setPlotHeight(int height) {
      if (ui->plot->height() == height) {
        return;
      }
      ui->plot->setMinimumHeight(height);
      ui->plot->setMaximumHeight(height);
      ui->plot->replot();
    }




  private: // ~~~~ plot range functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setXAxisRangeFields(const QCPRange& range) {
      if (std::abs(ui->xAxisLowerField->value() - range.lower) > kDiffThreshold) {
        ui->xAxisLowerField->setValue(range.lower);
      }
      if (std::abs(ui->xAxisUpperField->value() - range.upper) > kDiffThreshold) {
        ui->xAxisUpperField->setValue(range.upper);
      }
    }


    void setXAxisLowerRange(double value) {
      if (ui->plot->xAxis->range().lower == value) {
        return;
      }
      ui->plot->xAxis->setRangeLower(value);
      ui->plot->replot();
    }


    void setXAxisUpperRange(double value) {
      if (ui->plot->xAxis->range().upper == value) {
        return;
      }
      ui->plot->xAxis->setRangeUpper(value);
      ui->plot->replot();
    }


    void setYAxisRangeFields(const QCPRange& range) {
      if (std::abs(ui->yAxisLowerField->value() - range.lower) > kDiffThreshold) {
        ui->yAxisLowerField->setValue(range.lower);
      }
      if (std::abs(ui->yAxisUpperField->value() - range.upper) > kDiffThreshold) {
        ui->yAxisUpperField->setValue(range.upper);
      }
    }


    void setYAxisLowerRange(double value) {
      if (ui->plot->yAxis->range().lower == value) {
        return;
      }
      ui->plot->yAxis->setRangeLower(value);
      ui->plot->replot();
    }


    void setYAxisUpperRange(double value) {
      if (ui->plot->yAxis->range().upper == value) {
        return;
      }
      ui->plot->yAxis->setRangeUpper(value);
      ui->plot->replot();
    }


    void setYAxis2RangeFields(const QCPRange& range) {
      if (std::abs(ui->yAxis2LowerField->value() - range.lower) > kDiffThreshold) {
        ui->yAxis2LowerField->setValue(range.lower);
      }
      if (std::abs(ui->yAxis2UpperField->value() - range.upper) > kDiffThreshold) {
        ui->yAxis2UpperField->setValue(range.upper);
      }
    }


    void setYAxis2LowerRange(double value) {
      if (ui->plot->yAxis2->range().lower == value) {
        return;
      }
      ui->plot->yAxis2->setRangeLower(value);
      ui->plot->replot();
    }


    void setYAxis2UpperRange(double value) {
      if (ui->plot->yAxis2->range().upper == value) {
        return;
      }
      ui->plot->yAxis2->setRangeUpper(value);
      ui->plot->replot();
    }




  private: // ~~~~ tick count function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setXAxisTickCount(int count) {
      ui->plot->xAxis->ticker()->setTickCount(count);
      ui->plot->replot();
    }


    void setXAxisForceCountPlot(bool is_force) {
      auto ticker = QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed);
      if (is_force) {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      } else {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssReadability);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      }
      ticker.get()->setTickCount(ui->xAxisTickCountField->value());
      ticker.get()->setTickStep(std::pow(10, -ui->plot->xAxis->numberPrecision()));
      ui->plot->xAxis->setTicker(ticker);
      ui->plot->replot();
    }


    void setYAxisTickCount(int count) {
      ui->plot->yAxis->ticker()->setTickCount(count);
      ui->plot->replot();
    }


    void setYAxisForceCountPlot(bool is_force) {
      auto ticker = QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed);
      if (is_force) {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      } else {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssReadability);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      }
      ticker.get()->setTickCount(ui->yAxisTickCountField->value());
      ticker.get()->setTickStep(std::pow(10, -ui->plot->yAxis->numberPrecision()));
      ui->plot->yAxis->setTicker(ticker);
      ui->plot->replot();
    }


    void setYAxis2TickCount(int count) {
      ui->plot->yAxis2->ticker()->setTickCount(count);
      ui->plot->replot();
    }


    void setYAxis2ForceCountPlot(bool is_force) {
      auto ticker = QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed);
      if (is_force) {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssMeetTickCount);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      } else {
        ticker.get()->setTickStepStrategy(QCPAxisTicker::TickStepStrategy::tssReadability);
        ticker.get()->setScaleStrategy(QCPAxisTickerFixed::ScaleStrategy::ssMultiples);
      }
      ticker.get()->setTickCount(ui->yAxisTickCountField->value());
      ticker.get()->setTickStep(std::pow(10, -ui->plot->yAxis->numberPrecision()));
      ui->plot->yAxis2->setTicker(ticker);
      ui->plot->replot();
    }




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::PlotPanel* ui;
};
































/*
================================================================================
================================================================================
==
==        PlotPanel Definition
==
================================================================================
================================================================================
*/


PlotPanel::PlotPanel(QWidget* parent, const std::string& plotName)
      : QWidget(parent), ui(new Ui::PlotPanel),
        name(plotName) {
  ui->setupUi(this);
  handler = new SignalHandler(ui);
  if (name.empty()) {
    ui->plotTitleLabel->hide();
  } else {
    ui->plotTitleLabel->setText(QString::fromStdString(plotName));
  }
}


PlotPanel::~PlotPanel() {
  delete handler;
  delete ui;
}








/*
********************************************************************************
**        Setup functions
********************************************************************************
*/


namespace {


inline QColor getColor(int i, int size) {
  QColor color {};
  color.setHsvF((float) ((double) i / size), 1, 0.7);
  return color;
}


}




void PlotPanel::setupPlot(
      const std::string& name_x,
      const std::string& name_y1, const std::vector<std::string>& slist_1,
      const std::string& name_y2, const std::vector<std::string>& slist_2) {
  ui->plotWidthField->setValue(ui->plot->width());
  ui->plotHeightField->setValue(ui->plot->height());
  // initialise graphs
  int numSeries = slist_1.size() + slist_2.size();
  for (const auto& name : slist_1) {
    ilist_1.push_back(ui->plot->graphCount());
    auto graph = ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
    graph->setName(QString::fromStdString(name));
    graph->setPen(getColor(ilist_1.back(), numSeries));
    graph->setAntialiased(true);
  }
  for (const auto& name : slist_2) {
    ilist_2.push_back(ui->plot->graphCount());
    auto graph = ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2);
    graph->setName(QString::fromStdString(name));
    graph->setPen(getColor(ilist_2.back(), numSeries));
    graph->setAntialiased(true);
  }
  // set up legend
  QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  ui->plot->plotLayout()->addElement(0, 1, subLayout);
  ui->plot->plotLayout()->addElement(0, 2, new QCPLayoutElement);
  subLayout->addElement(0, 0, new QCPLayoutElement);
  subLayout->addElement(1, 1, ui->plot->legend);
  subLayout->setRowStretchFactor(1, 0.001);
  subLayout->addElement(2, 2, new QCPLayoutElement);
  ui->plot->plotLayout()->setColumnStretchFactor(1, 0.001);
  ui->plot->plotLayout()->setColumnStretchFactor(2, 0.001);
  ui->plot->legend->setVisible(true);
  // setup axis
  ui->plot->xAxis->setLabel(QString::fromStdString(name_x));
  ui->plot->yAxis->setLabel(QString::fromStdString(name_y1));
  ui->plot->yAxis2->setVisible(!name_y2.empty());
  ui->plot->yAxis2->setLabel(QString::fromStdString(name_y2));
  ui->plot->replot();
}








/*
********************************************************************************
**        Accessors / Adders
********************************************************************************
*/


template<>
void PlotPanel::addPoint<PlotPanel::Axis::axMain>(double x, double y, int i) {
  auto graph = ui->plot->graph(ilist_1.at(i));
  graph->addData(x, y);
  ui->plot->rescaleAxes();
  ui->plot->replot();
}


template<>
void PlotPanel::addPoint<PlotPanel::Axis::axSub>(double x, double y, int i) {
  auto graph = ui->plot->graph(ilist_2.at(i));
  graph->addData(x, y);
  ui->plot->rescaleAxes();
  ui->plot->replot();
}




template<>
void PlotPanel::addPoint<PlotPanel::Axis::axMain>(
      const std::vector<std::pair<double, double>>& points, int i) {
  auto graph = ui->plot->graph(ilist_1.at(i));
  for (const auto& p : points) {
    graph->addData(p.first, p.second);
  }
  ui->plot->rescaleAxes();
  ui->plot->replot();
}


template<>
void PlotPanel::addPoint<PlotPanel::Axis::axSub>(
      const std::vector<std::pair<double, double>>& points, int i) {
  auto graph = ui->plot->graph(ilist_2.at(i));
  for (const auto& p : points) {
    graph->addData(p.first, p.second);
  }
  ui->plot->rescaleAxes();
  ui->plot->replot();
}








/*
********************************************************************************
**        Functions
********************************************************************************
*/


bool PlotPanel::savePng(const std::string& dirPath, const std::string& prefix) {
  if (name.empty() && prefix.empty()) {
    throw std::runtime_error("Empty name");
  }
  std::string fileName = prefix;
  if (prefix.empty()) {
    fileName += name;
  } else if (!name.empty()) {
    fileName += " - " + name;
  }
  return ui->plot->savePng(
      QString::fromStdString(dirPath + "/" + fileName + ".png"));
}
