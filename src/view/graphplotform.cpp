#include "view/graphplotform.h"
#include "./ui_graphplotform.h"

#include <cmath>
#include <limits>
#include <vector>


namespace { // BEGIN OF NAMESPACE <GLOBAL::UNNAMED> ============================


constexpr int kBaseDecimals = 2;

constexpr double kZeroAreaPortion = 0.2;
constexpr double kEndSpacePortion = 0.1;


int getFieldDecimals(double min, double max) {
  int pow = kBaseDecimals;
  for (double diff = max - min; diff < 1 && diff != 0; diff *= 10) {
    ++pow;
  }
  return pow;
}


double correctLowerValue(double lv, double uv, double corrDiff, int pow) {
  double fac = std::pow(10, pow);
  int clv = std::floor(std::ceil(uv * fac) - corrDiff);
  clv += 10 - std::abs(clv % 10);
  if (clv > std::ceil(lv * fac)) {
    clv = std::ceil(lv * fac);
    clv -= std::abs(clv % 10);
  }
  return clv / fac;
}


double correctUpperValue(double lv, double uv, double corrDiff, int pow) {
  double fac = std::pow(10, pow);
  int cuv = std::floor(std::ceil(lv * fac) + corrDiff);
  cuv -= std::abs(cuv % 10);
  if (cuv < std::ceil(uv * fac)) {
    cuv = std::ceil(uv * fac);
    cuv += 10 - std::abs(cuv % 10);
  }
  return cuv / fac;
}


void adjustAxisRange(QCPAxis* axis) {
  auto r = axis->range();
  double diff = r.upper - r.lower;
  if (diff == 0) {
    return;
  }

  // find inverse power to get ceil of diff to at least a 10
  // 10 so that modulo operator works
  int pow = 0;
  for (double num = diff; std::ceil(num) < 10; num *= 10) {
    ++pow;
  }

  double lower = r.lower;
  double upper = r.upper;
  double corrDiff = std::ceil(diff * std::pow(10, pow)) * (1+kEndSpacePortion);
  if (upper < 0 || lower < 0) {
    // correct lower
    lower = correctLowerValue(r.lower, r.upper, corrDiff, pow);
    // correct upper
    if (diff*kZeroAreaPortion > -upper || upper == 0) {
      upper = 0;
    } else {
      upper = correctUpperValue(r.lower, r.upper, corrDiff, pow);
    }
  } else {
    // correct lower
    if (diff*kZeroAreaPortion > lower || lower == 0) {
      lower = 0;
    } else {
      lower = correctLowerValue(r.lower, r.upper, corrDiff, pow);
    }
    // correct upper
    upper = correctUpperValue(r.lower, r.upper, corrDiff, pow);
  }

  axis->setRange(lower, upper);
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
  adjustAxisRange(ui->plot->xAxis);
  adjustAxisRange(ui->plot->yAxis);
  ui->plot->xAxis->setLabel(QString::fromStdString(xHdr));
  ui->plot->yAxis->setLabel(QString::fromStdString(yHdr));
  ui->plot->replot();
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
