#pragma once

#include <QtCore/qfuturewatcher.h>
#include <QDialog>

#include "driver/vrfbdriver.hpp"
#include "driver/vrfbdriver_io.hpp"
#include "logger.hpp"




QT_BEGIN_NAMESPACE
namespace Ui {
  class SCSimReportPopup;
}
QT_END_NAMESPACE




/**
 * `QDialog` to present shunt simulation data as it is being simulated.
*/
class SCSimReportPopup : public QDialog {
  Q_OBJECT

  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCSimReportPopup(QWidget* parent = nullptr);
    ~SCSimReportPopup();


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Starts the given job.
     *
     * @param j Job to start and track.
    */
    void start(const vrfbdriver::shuntcur::ShuntSimJob& j);


  signals:
    void simulationSuccess();                           // Emitted when simulation completed successfully.
    void simulationFailed(const std::string& msg);      // Emitted when an error occured during simulation.


  private:
    void start();

    void reportReadyAt(int);
    void displayReport();
    void displayFailed(const std::string& msg);

    void deleteSelf(int);


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCSimReportPopup* ui;

    vrfbdriver::shuntcur::ShuntSimJob* simJob;
    vrfbdriver::shuntcur::ShuntSimReport report;
    vrfbdriver::io::shuntcur::ShuntSimStepIO* stepIO;
    vrfbdriver::io::shuntcur::ShuntSimReportIO* reportIO;

    QFutureWatcher<vrfbdriver::shuntcur::ShuntSimStep> watcher;
    QThreadPool pool;

    int iter = 0;
};
