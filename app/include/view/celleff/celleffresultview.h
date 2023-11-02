#pragma once

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
    CEResultView(QWidget*);
    ~CEResultView();


  signals:
    void exportRequested(CEResultView*);


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void plotGraphs(const std::vector<vrfbdriver::PerformanceEntry_CE>&);
    bool exportImages(logger::Logger&);


  private:
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
    std::vector<PlotConfig> plotCfgs;


  private slots:
    void on_exportBtn_clicked();
};
