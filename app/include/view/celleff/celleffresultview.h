#pragma once

#include <QtCore/qfuturewatcher.h>
#include <QDialog>

#include <string>
#include <utility>
#include <vector>

#include "logger.hpp"
#include "driver/vrfbdriver.hpp"
#include "view/plotpanel.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEResultView;
}
QT_END_NAMESPACE


class CEResultView : public QDialog {
  Q_OBJECT


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    CEResultView(logger::Logger* logger, QWidget* parent=nullptr);
    ~CEResultView();




  signals:
    void exportRequest();
    void exportSuccess();
    void exportFailure();




  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void plotGraphs(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    bool exportGraphs(logger::Logger&);


  private:
    void handleExportRequest();
    void handleExportSuccess();
    void handleExportFailure();
    void deleteSelf(int) {delete this;}




  private: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    struct PlotConfig {
      std::string title;
      std::string xHdr;
      std::string yHdr;
      PlotPanel* panel=nullptr;
    };




  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::CEResultView* ui;

    logger::Logger* l;
    std::vector<PlotConfig> plotCfgs;

    QFutureWatcher<void> watcher;
    QThreadPool pool;


  private slots:
    void on_exportBtn_clicked();
};
