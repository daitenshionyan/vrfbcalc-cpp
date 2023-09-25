#pragma once

#include <QDialog>

#include "vrfblib/vrfblib.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCConfigPopup;
}
QT_END_NAMESPACE


class SCConfigPopup : public QDialog {
  Q_OBJECT

  public:
    class ConfigForm : public QWidget {
      public:
        ConfigForm(QWidget* parent) : QWidget(parent) {}

        virtual ~ConfigForm() = default;
        virtual vrfb::shuntcur::ParamGenerator* getGenerator() = 0;
    };


    struct Job {
      std::string name;
      double chgCurr;
      vrfb::shuntcur::ParamGenerator* gen;

      ~Job() {
        delete gen;
      }
    };


  public:
    SCConfigPopup(QWidget* parent);
    ~SCConfigPopup();
    Job getJob();


  private:
    Ui::SCConfigPopup* ui;
    ConfigForm* form;
};
