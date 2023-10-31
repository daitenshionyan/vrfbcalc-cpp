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
    void start(const vrfbdriver::ShuntSimJob&);


  signals:
    void simulationFailed(const std::string& msg);


  private:
    void reportReadyAt(int);
    void displayReport();
    void displayFailed(const std::string& msg);

    void deleteSelf(int) {delete this;}


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimReportPopup* ui;

    vrfbdriver::ShuntSimReport report;

    QFutureWatcher<vrfbdriver::ShuntSimStep> watcher;
    QThreadPool pool;
};
