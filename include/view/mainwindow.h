#pragma once

#include <string>

#include <QtCore/qfuturewatcher.h>
#include <QMainWindow>

#include "view/celleff/celleffconfigpopup.h"
#include "view/celleff/celleffresultview.h"
#include "driver/vrfbdriver.hpp"
#include "logger.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow, private logger::Logger {
  Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


  signals:
    void availableLogMsg(const logger::LogMsg&);
  signals:
    void completedPerformanceReading(const std::vector<vrfbdriver::PerformanceEntry_CE>&);


  private:
    void startCalc();
    void log(const logger::LogMsg&) override;
    void logMsg(const logger::LogMsg&);
    inline void logMsgAt(int index) {
      log(watcher.future().resultAt(index));
    }
    void disableActions();
    void enableActions();

    void displayPerformanceView(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    void exportCEPerformance(CEResultView*);

    Ui::MainWindow* ui;
    CEConfigPopup* popup_ce;

    QFutureWatcher<logger::LogMsg> watcher;
    QThreadPool pool;


  private slots:
    void on_action_openOutput_triggered(bool);
    void on_actionCECalculations_triggered(bool);
    void on_actionCEAnalysis_triggered(bool);
};
