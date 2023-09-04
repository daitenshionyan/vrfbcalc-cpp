#pragma once

#include <QMainWindow>

#include "view/celleffconfigpopup.h"
#include "vrfbdriver.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow, public vrfbdriver::Writer {
  Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void writeln(const std::string& text = "") override;

    void writeln_succ(const std::string&) override;
    void writeln_warn(const std::string&) override;
    void writeln_fail(const std::string&) override;


  private slots:
    void on_startBtn_clicked();
    void on_cfgBtn_clicked();


  private:
    Ui::MainWindow* ui;
    CEConfigPopup* popup_ce;
};
