#pragma once

#include <QtCore/qfuturewatcher.h>
#include <QMainWindow>


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


  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class LoggerManager;
    class DriverManager;


    Ui::MainWindow* ui;
    LoggerManager* logger;
    DriverManager* driver;


  private slots:
    void on_action_openOutput_triggered(bool);
    void on_actionCECalculations_triggered(bool);
    void on_actionCEAnalysis_triggered(bool);

    void on_actionSCSimulate_triggered(bool);
};
