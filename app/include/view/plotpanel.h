#pragma once

#include <string>
#include <vector>

#include <QWidget>




QT_BEGIN_NAMESPACE
namespace Ui {
  class PlotPanel;
}
QT_END_NAMESPACE




class PlotPanel : public QWidget {
  Q_OBJECT


  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    enum class Axis {axMain, axSub};


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PlotPanel(QWidget* parent=nullptr, const std::string& plotName="");
    ~PlotPanel();


  public: // ~~~~ setup functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setupPlot(
          const std::string& name_x,
          const std::string& name_y1, const std::vector<std::string>& slist_1,
          const std::string& name_y2="", const std::vector<std::string>& slist_2={});



  public: // ~~~~ accessors / adders ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    template<Axis Ax=Axis::axMain>
    void addPoint(double x, double y, int i=0);

    template<Axis Ax=Axis::axMain>
    void addPoint(const std::vector<std::pair<double, double>>& points, int i=0);


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool savePng(const std::string& dirPath, const std::string& prefix="");


  private:
    class SignalHandler;


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::PlotPanel* ui;
    SignalHandler* handler;

    std::string name;

    std::vector<int> ilist_1;       // graph indexes using main y axis
    std::vector<int> ilist_2;       // graph indexes using sub y axis
};
