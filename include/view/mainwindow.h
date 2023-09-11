#pragma once

#include <string>

#include <QtCore/qfuturewatcher.h>
#include <QMainWindow>

#include "view/celleffconfigpopup.h"
#include "driver/vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow, public vrfbdriver::Writer {
  Q_OBJECT

  public:
    enum class msg_state {kSucc, kInfo, kWarn, kFail};
    struct log_msg {
      msg_state state;
      std::string msg;
    };

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void writeln(const std::string& text = "") override;
    void writeln_succ(const std::string&) override;
    void writeln_warn(const std::string&) override;
    void writeln_fail(const std::string&) override;


  private:
    void logMsgAt(int index);

    Ui::MainWindow* ui;
    CEConfigPopup* popup_ce;

    QFutureWatcher<log_msg> watcher;
    QThreadPool pool;


  private slots:
    void on_action_openOutput_triggered(bool);
    void on_startBtn_clicked();
    void on_cfgBtn_clicked();
};
