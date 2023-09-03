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

  void write(const std::string&) override;
  void writeln(const std::string&) override;


 private slots:
  void on_startBtn_clicked();
  void on_cfgBtn_clicked();


 private:
  Ui::MainWindow* ui;
  CellEffConfigPopup* popup_ce;
};
