#pragma once

#include <string>

#include <QtCore/qfuturewatcher.h>
#include <QMainWindow>

#include "view/celleff/celleffconfigpopup.h"
#include "view/celleff/celleffresultview.h"
#include "view/shuntcur/shuntcursimconfigpopup.h"
#include "view/shuntcur/shuntcursimreportpopup.h"
#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
  Q_OBJECT


  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


  signals:
    void availableLogMsg(const logger::LogMsg&);
    void completedPerformanceReading(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    void jobReadySCSim(const vrfbdriver::shuntcur::ShuntSimJob&);


  private: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class AppLogger;
    class AppDriver;


    Ui::MainWindow* ui;
    AppLogger* logger;
    AppDriver* driver;


  private slots:
    void on_action_openOutput_triggered(bool);
    void on_actionCECalculations_triggered(bool);
    void on_actionCEAnalysis_triggered(bool);

    void on_actionSCSimulate_triggered(bool);
};
