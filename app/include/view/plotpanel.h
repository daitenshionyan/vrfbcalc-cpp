#pragma once

#include <string>
#include <vector>

#include <QWidget>




QT_BEGIN_NAMESPACE
namespace Ui {
  class PlotPanel;
}
QT_END_NAMESPACE




/**
 * Panel to manage parameters of a plot through GUI interactions.
*/
class PlotPanel : public QWidget {
  Q_OBJECT


  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    enum class Axis {axMain, axSub};


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    PlotPanel(QWidget* parent=nullptr, const std::string& plotName="");
    ~PlotPanel();


  public: // ~~~~ setup functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Sets up the plot. If `name_y2` is an empty string, the secondary axis
     * will be hidden.
     *
     * @param name_x X-Axis name.
     * @param name_y1 Primary Y-Axis name.
     * @param slist_1 List of series names that uses the primary axis.
     * @param name_y2 Secondary Y-Axis name.
     * @param slist_2 List of series names that uses the secondary axis.
    */
    void setupPlot(
          const std::string& name_x,
          const std::string& name_y1, const std::vector<std::string>& slist_1,
          const std::string& name_y2="", const std::vector<std::string>& slist_2={});



  public: // ~~~~ accessors / adders ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Adds a point to the specified series.
     *
     * @param <Ax> Axis of series to which the point is to be added to.
     * @param x X value of point.
     * @param y Y value of point.
     * @param i Index of series, as given in the latest call to
     *    `PlotPanel::setupPlot()`, to add the point to.
    */
    template<Axis Ax=Axis::axMain>
    void addPoint(double x, double y, int i=0);

    /**
     * Adds a list of points to the specified series.
     *
     * @param <Ax> Axis of series to which the points are to be added to.
     * @param points Points to be added to series.
     * @param i Index of series, as given in the lastes call to
     *    'PlotPanel::setupPlot()`, to add the points to.
    */
    template<Axis Ax=Axis::axMain>
    void addPoint(const std::vector<std::pair<double, double>>& points, int i=0);


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Saves the plot in PNG format. The file name will be
     * `${prefix} - ${plotName}.png`. If given prefix is or plotName is an empty
     * string, the dash `-` symbol will be omitted.
     *
     * @param dirPath Path to directory to save to.
     * @param prefix Prefix string to add to file name.
    */
    bool savePng(const std::string& dirPath, const std::string& prefix="");

    void clearData();


  private:
    class SignalHandler;


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::PlotPanel* ui;
    SignalHandler* handler;

    std::string name;

    std::vector<int> ilist_1;       // graph indexes using main y axis
    std::vector<int> ilist_2;       // graph indexes using sub y axis
};
