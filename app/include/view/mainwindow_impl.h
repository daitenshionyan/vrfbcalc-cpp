#pragma once

#include "view/mainwindow.h"

#include <QtCore/qfuturewatcher.h>
#include <QObject>
#include <QWidget>

#include <vector>

#include "logger.hpp"
#include "view/celleff/celleffconfigpopup.h"
#include "view/celleff/celleffresultview.h"
#include "view/shuntcur/shuntcursimconfigpopup.h"

#include "driver/vrfbdriver.hpp"
































/*
================================================================================
================================================================================
==
==        LoggerManager
==
================================================================================
================================================================================
*/


class MainWindow::LoggerManager
      : public QObject, public logger::Logger {
  Q_OBJECT

  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    LoggerManager(Ui::MainWindow* uip);

    LoggerManager() = delete;
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager(LoggerManager&&) = default;

    LoggerManager& operator=(const LoggerManager&) = delete;
    LoggerManager& operator=(LoggerManager&&) = default;

    ~LoggerManager() = default;




  signals:
    void availableLogMsg(const logger::LogMsg&);




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void log(const logger::LogMsg& msg) override;


  private:
    void writeLogMsg(const logger::LogMsg& lm);




  private:
    Ui::MainWindow* ui;
};
































/*
================================================================================
================================================================================
==
==        DriverManager
==
================================================================================
================================================================================
*/


class MainWindow::DriverManager : public QObject {
  Q_OBJECT


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    DriverManager(QWidget* parent, Ui::MainWindow* uip, logger::Logger* logger);

    DriverManager() = delete;
    DriverManager(const DriverManager&) = delete;
    DriverManager(DriverManager&&) = default;

    DriverManager& operator=(const DriverManager&) = delete;
    DriverManager& operator=(DriverManager&&) = default;

    ~DriverManager();




  signals:
    void completedPerformanceReading(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    void jobReadySCSim(const vrfbdriver::shuntcur::ShuntSimJob&);




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void enableActions();
    void disableActions();




  public: // ~~~~ cell efficiency functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void showConfigPopup_CE();
    void analyseResults_CE();


  private:
    void constructConfigPopup_CE();
    void startCalc_CE();
    void displayPerformanceView(const std::vector<vrfbdriver::PerformanceEntry_CE>&);




  public: // ~~~~ shunt current functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void showConfigPopup_SE();


  private:
    void constructConfigPopup_SC();
    void startSim_SC();
    void displayReport_SCSim(const vrfbdriver::shuntcur::ShuntSimJob& j);



  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QWidget* p;
    Ui::MainWindow* ui;
    logger::Logger* l;

    CEConfigPopup* config_ce=nullptr;
    SCSimConfigPopup* config_scSim=nullptr;

    QFutureWatcher<void> watcher;
    QThreadPool pool;
};
