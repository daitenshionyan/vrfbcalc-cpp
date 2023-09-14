#pragma once


#include <vector>

#include <QDialog>

#include "table.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
  class CEResultView;
}
QT_END_NAMESPACE


class CEResultView : public QDialog {
  Q_OBJECT

  public:
    CEResultView(QWidget*);
    ~CEResultView();

    void createGraphs(const std::vector<vrfb::Table>&);


  private:
    void deleteSelf(int) {delete this;}

    Ui::CEResultView* ui;
};
