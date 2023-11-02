#pragma once

#include <QtCore/qfuturewatcher.h>
#include <QDialog>

#include "driver/vrfbdriver.hpp"
#include "logger.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimReportPopup;
}
QT_END_NAMESPACE


class SCSimReportPopup : public QDialog {
  Q_OBJECT

  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimReportPopup(QWidget* parent = nullptr);
    ~SCSimReportPopup();


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void start(const vrfbdriver::shuntcur::ShuntSimJob&);


  signals:
    void simulationFailed(const std::string& msg);


  private:
    void reportReadyAt(int);
    void displayReport();
    void displayFailed(const std::string& msg);

    void deleteSelf(int);


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimReportPopup* ui;

    vrfbdriver::shuntcur::ShuntSimReport report;

    QFutureWatcher<vrfbdriver::shuntcur::ShuntSimStep> watcher;
    QThreadPool pool;
};
