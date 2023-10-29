#include "view/graphplotform.h"
#include "./ui_graphplotform.h"

#include <cmath>
#include <limits>
#include <vector>


namespace { // BEGIN OF NAMESPACE <GLOBAL::UNNAMED> ============================


constexpr int kBaseDecimals = 2;


int getFieldDecimals(double min, double max) {
  int pow = kBaseDecimals;
  for (double diff = max - min; diff < 1 && diff != 0; diff *= 10) {
    ++pow;
  }
  return pow;
}


} // END OF NAMESPACE <GLOBAL::UNNAMED> ----------------------------------------
// namespace <GLOBAL>


// BEGIN OF CLASS <GraphPlotForm::SignalHandler> ===============================
class GraphPlotForm::SignalHandler : public QObject {
  public:
    SignalHandler(Ui::GraphPlotForm* uip) : ui(uip) {
      // dimension fields -> plot dimensions
      connect(ui->plotWidthField, &QSpinBox::valueChanged,
          this, &SignalHandler::setWidthPlot);
      connect(ui->plotHeightField, &QSpinBox::valueChanged,
          this, &SignalHandler::setHeightPlot);
      // range fields -> plot axis range
      connect(ui->xAxisLowerField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setXAxisLowerPlot);
      connect(ui->xAxisUpperField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setXAxisUpperPlot);
      connect(ui->yAxisLowerField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxisLowerPlot);
      connect(ui->yAxisUpperField, &QDoubleSpinBox::valueChanged,
          this, &SignalHandler::setYAxisUpperPlot);
      // plot axis range -> range fields
      connect(ui->plot->xAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
          this, &SignalHandler::setXAxisRangeFields);
      connect(ui->plot->yAxis, static_cast<void (QCPAxis::*)(const QCPRange&)>(&QCPAxis::rangeChanged),
          this, &SignalHandler::setYAxisRangeFields);
      // tick count fields -> plot
      connect(ui->xAxisTickCountField, &QSpinBox::valueChanged,
          this, &SignalHandler::setXAxisTickCount);
      connect(ui->yAxisTickCountField, &QSpinBox::valueChanged,
          this, &SignalHandler::setYAxisTickCount);
      connect(ui->xAxisForceCountCB, &QCheckBox::toggled,
          this, &SignalHandler::setXAxisForceCountPlot);
      connect(ui->yAxisForceCountCB, &QCheckBox::toggled,
          this, &SignalHandler::setYAxisForceCountPlot);
    }


    inline void setXDiffThreshold(double value) {
      xDiffThreshold = value;
    }

    inline void setYDiffThreshold(double value) {
      yDiffThreshold = value;
    }


  private: // :::: fields slots ::::::::::::::::::::::::::::::::::::::::::::::::
    void setXAxisRangeFields(const QCPRange& range) {
      if (std::abs(ui->xAxisLowerField->value() - range.lower) >= xDiffThreshold) {
        ui->xAxisLowerField->setValue(range.lower);
      }
      if (std::abs(ui->xAxisUpperField->value() - range.upper) >= xDiffThreshold) {
        ui->xAxisUpperField->setValue(range.upper);
      }
    }


    void setYAxisRangeFields(const QCPRange& range) {
      if (std::abs(ui->yAxisLowerField->value() - range.lower) >= yDiffThreshold) {
        ui->yAxisLowerField->setValue(range.lower);
      }
      if (std::abs(ui->yAxisUpperField->value() - range.upper) >= yDiffThreshold) {
        ui->yAxisUpperField->setValue(range.upper);
      }
    }


  private: // :::: plot slots ::::::::::::::::::::::::::::::::::::::::::::::::::
    void setWidthPlot(int width) {
      if (ui->plot->width() == width) {
        return;
      }
      ui->plot->setMinimumWidth(width);
      ui->plot->setMaximumWidth(width);
      ui->plot->replot();
    }


    void setHeightPlot(int height) {
      if (ui->plot->height() == height) {
        return;
      }
      ui->plot->setMinimumHeight(height);
      ui->plot->setMaximumHeight(height);
      ui->plot->replot();
    }


    void setXAxisLowerPlot(double value) {
      if (ui->plot->xAxis->range().lower == value) {
        return;
      }
      ui->plot->xAxis->setRangeLower(value);
      ui->plot->replot();
    }


    void setXAxisUpperPlot(double value) {
      if (ui->plot->xAxis->range().upper == value) {
        return;
      }
      ui->plot->xAxis->setRangeUpper(value);
      ui->plot->replot();
    }


    void setYAxisLowerPlot(double value) {
      if (ui->plot->yAxis->range().lower == value) {
        return;
      }
      ui->plot->yAxis->setRangeLower(value);
      ui->plot->replot();
    }


    void setYAxisUpperPlot(double value) {
      if (ui->plot->yAxis->range().upper == value) {
        return;
      }
      ui->plot->yAxis->setRangeUpper(value);
      ui->plot->replot();
    }


    void setXAxisTickCount(int count) {
      ui->plot->xAxis->ticker()->setTickCount(count);
      ui->plot->replot();
    }


    void setYAxisTickCount(int count) {
      ui->plot->yAxis->ticker()->setTickCount(count);
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


  private:
    double xDiffThreshold = 1;
    double yDiffThreshold = 1;

    Ui::GraphPlotForm* ui;
};  // END OF CLASS <GraphPlotForm::SignalHandler> -----------------------------
// namespace <GLOBAL>


GraphPlotForm::GraphPlotForm(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::GraphPlotForm) {
  ui->setupUi(this);
  handler = new SignalHandler(ui);
}


GraphPlotForm::~GraphPlotForm() {
  delete handler;
  delete ui;
}


void GraphPlotForm::setupPlot(
      const std::vector<vrfbdriver::PerformanceEntry_CE>& entries,
      const std::string& xHdr, const std::string& yHdr) {
  ui->plotWidthField->setValue(ui->plot->width());
  ui->plotHeightField->setValue(ui->plot->height());
  if (entries.empty()) {
    return;
  }

  double min_x = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();

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

  std::size_t i_series = 0;
  for (const vrfbdriver::PerformanceEntry_CE& entry : entries) {
    auto graph = ui->plot->addGraph();

    // set graph style
    QColor color {};
    color.setHsvF((float) ((double) i_series / entries.size()), 1, 0.7);
    graph->setPen(color);
    graph->setName(QString::fromStdString(entry.name));
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCross));
    graph->setAntialiased(true);

    // add graph data
    for (std::size_t i = 0; i < entry.table.numRows(); ++i) {
      double x = entry.table.get<double>(xHdr, i);
      double y = entry.table.get<double>(yHdr, i);
      graph->addData(x, y);
      min_x = (min_x < x) ? min_x : x;
      max_x = (max_x < x) ? x : max_x;
      min_y = (min_y < y) ? min_y : y;
      max_y = (max_y < y) ? y : max_y;
    }
    ++i_series;
  }

  // set up fields
  setupXFields(min_x, max_x);
  setupYFields(min_y, max_y);

  // set initial range
  ui->plot->xAxis->setRange(min_x, max_x);
  ui->plot->yAxis->setRange(min_y, max_y);
  // adjustAxisRange(ui->plot->xAxis);
  // adjustAxisRange(ui->plot->yAxis);
  ui->plot->xAxis->setLabel(QString::fromStdString(xHdr));
  ui->plot->yAxis->setLabel(QString::fromStdString(yHdr));
  ui->plot->replot();
}


void GraphPlotForm::setupPlot(const std::vector<Series>& slist,
      const std::string& name_x, std::string name_y) {
  ui->plotWidthField->setValue(ui->plot->width());
  ui->plotHeightField->setValue(ui->plot->height());
  if (slist.empty()) {
    return;
  }

  double min_x = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();

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

  std::size_t i_series = 0;
  for (const auto& s : slist) {
    auto graph = ui->plot->addGraph();

    // set graph style
    QColor color {};
    color.setHsvF((float) ((double) i_series / slist.size()), 1, 0.7);
    graph->setPen(color);
    graph->setName(QString::fromStdString(s.name));
    graph->setAntialiased(true);

    // add graph data
    for (std::size_t i = 0; i < s.xs.size(); ++i) {
      double x = s.xs.at(i);
      double y = s.ys.at(i);
      graph->addData(x, y);
      min_x = (min_x < x) ? min_x : x;
      max_x = (max_x < x) ? x : max_x;
      min_y = (min_y < y) ? min_y : y;
      max_y = (max_y < y) ? y : max_y;
    }
    ++i_series;
  }

  // set up fields
  setupXFields(min_x, max_x);
  setupYFields(min_y, max_y);

  // set initial range
  ui->plot->xAxis->setRange(min_x, max_x);
  ui->plot->yAxis->setRange(min_y, max_y);
  ui->plot->xAxis->setLabel(QString::fromStdString(name_x));
  ui->plot->yAxis->setLabel(QString::fromStdString(name_y));
  ui->plot->replot();
}


void GraphPlotForm::setupPlot(
      const std::vector<double>& series_x,
      const std::vector<double>& series_y,
      const std::string& name_x, const std::string& name_y) {
  ui->plotWidthField->setValue(ui->plot->width());
  ui->plotHeightField->setValue(ui->plot->height());

  double min_x = std::numeric_limits<double>::max();
  double max_x = -std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_y = -std::numeric_limits<double>::max();

  auto graph = ui->plot->addGraph();
  for (std::size_t i = 0; i < series_x.size() && i < series_y.size(); ++i) {
    double x = series_x[i];
    double y = series_y[i];
    graph->addData(x, y);
    min_x = (min_x < x) ? min_x : x;
    max_x = (max_x < x) ? x : max_x;
    min_y = (min_y < y) ? min_y : y;
    max_y = (max_y < y) ? y : max_y;
  }

  // set up fields
  if (!series_x.empty()) {
    setupXFields(min_x, max_x);
    setupYFields(min_y, max_y);
  }

  // set initial range
  ui->plot->xAxis->setLabel(QString::fromStdString(name_x));
  ui->plot->yAxis->setLabel(QString::fromStdString(name_y));
  ui->plot->replot();
}


void GraphPlotForm::addPoint(double x, double y) {
  if (ui->plot->graphCount() == 0) {
    ui->plot->addGraph();
  }
  auto graph = ui->plot->graph(0);
  graph->addData(x, y);
  graph->rescaleAxes();
  ui->plot->replot();
}


int GraphPlotForm::dataCount() const {
  if (ui->plot->graphCount() == 0) {
    return 0;
  }
  return ui->plot->graph(0)->dataCount();
}


void GraphPlotForm::setupXFields(double min, double max) {
  int decimals = getFieldDecimals(min, max);
  handler->setXDiffThreshold(std::pow(10, -decimals));
  ui->xAxisLowerField->setDecimals(decimals);
  ui->xAxisUpperField->setDecimals(decimals);
  ui->xAxisTickCountField->setValue(ui->plot->xAxis->ticker()->tickCount());
}


void GraphPlotForm::setupYFields(double min, double max) {
  int decimals = getFieldDecimals(min, max);
  handler->setYDiffThreshold(std::pow(10, -decimals));
  ui->yAxisLowerField->setDecimals(decimals);
  ui->yAxisUpperField->setDecimals(decimals);
  ui->yAxisTickCountField->setValue(ui->plot->yAxis->ticker()->tickCount());
}


bool GraphPlotForm::savePng(const QString& path) {
  return ui->plot->savePng(path);
}
