#pragma once

#include "view/mainwindow.h"

#include <QObject>

#include "logger.hpp"








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
