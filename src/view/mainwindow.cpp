#include "view/mainwindow.h"
#include "./ui_mainwindow.h"

#include <filesystem>

#include <QtCore/qpromise.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QDesktopServices>

#include "strutils.hpp"
#include "vrfbcalccfg.hpp"


namespace {


class promise_writer : public vrfbdriver::Writer {
  public:
    promise_writer(QPromise<MainWindow::log_msg>& prom) : prom_p{&prom} {}

    void writeln(const std::string& text = "") override {
      prom_p->addResult(MainWindow::log_msg{MainWindow::msg_state::kInfo, text});
    }

    void writeln_succ(const std::string& text) override {
      prom_p->addResult(MainWindow::log_msg{MainWindow::msg_state::kSucc, text});
    }

    void writeln_warn(const std::string& text) override {
      prom_p->addResult(MainWindow::log_msg{MainWindow::msg_state::kWarn, text});
    }

    void writeln_fail(const std::string& text) override {
      prom_p->addResult(MainWindow::log_msg{MainWindow::msg_state::kFail, text});
    }


  private:
    QPromise<MainWindow::log_msg>* prom_p;
};


}


MainWindow::MainWindow(QWidget* parent)
      : QMainWindow(parent),
        ui(new Ui::MainWindow),
        popup_ce(new CEConfigPopup(this)) {
  ui->setupUi(this);
  ui->outputArea->appendHtml(
      QString("<p style=\"color:grey;white-space:pre\">")
      + QString::fromStdString(
            strutils::format_string(
                "%s\n%s\n%s",
                "================================================================================",
                vrfbcfg::licence_notice,
                "================================================================================"))
      + QString("</p>"));
  connect(&watcher, &QFutureWatcher<log_msg>::resultReadyAt,
    this, &MainWindow::on_resultReadyAt);
}


MainWindow::~MainWindow() {
  delete popup_ce;
  delete ui;
}


void MainWindow::on_action_openOutput_triggered(bool) {
  std::filesystem::path output_path {"output"};
  if (!std::filesystem::exists(output_path)) {
    writeln_fail("Output folder does not exist yet");
    return;
  } else if (!std::filesystem::is_directory(output_path)) {
    writeln_fail("The file 'output' exists but it is not a directory");
    return;
  }
  QDesktopServices::openUrl(QUrl::fromLocalFile("output"));
}


void MainWindow::on_startBtn_clicked() {
  if (watcher.isRunning()) {
    return;
  }
  watcher.setFuture(QtConcurrent::run([&](QPromise<log_msg>& p) {
    auto w = promise_writer{p};
    vrfbdriver::calcCellEff(popup_ce->getSetSupplierMap(), w);
  }));
}


void MainWindow::on_cfgBtn_clicked() {
  popup_ce->exec();
}


void MainWindow::writeln(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:black;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_succ(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:green;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_warn(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:orange;white-space:pre\">" + outText + "</p>");
}


void MainWindow::writeln_fail(const std::string& text) {
  QString outText = QString::fromStdString(
      strutils::format_string(
          "[%s] %s",
          strutils::getftime().c_str(), text.c_str()))
      .toHtmlEscaped();
  ui->outputArea->appendHtml("<p style=\"color:red;white-space:pre\">" + outText + "</p>");
}


void MainWindow::on_resultReadyAt(int index) {
  log_msg lm = watcher.future().resultAt(index);
  switch (lm.state) {
    case msg_state::kSucc:
      writeln_succ(lm.msg);
      break;
    case msg_state::kInfo:
      writeln(lm.msg);
      break;
    case msg_state::kWarn:
      writeln_warn(lm.msg);
      break;
    case msg_state::kFail:
      writeln_fail(lm.msg);
      break;
  }
}
