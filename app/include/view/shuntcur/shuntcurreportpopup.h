#pragma once

#include <QDialog>

#include "logger.hpp"

#include "view/graphplotformtitled.h"


QT_BEGIN_NAMESPACE
namespace Ui {
  class SCReportPopup;
}
QT_END_NAMESPACE


class SCReportPopup : public QDialog {
  Q_OBJECT


  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    enum class IndexingType {
      itCell, itStack, itLine
    };

    template<typename T>
    struct PlotConfig {
      using DataSupplier = double (T::*)(std::size_t) const;

      std::string name;
      IndexingType it;
      DataSupplier supplier = nullptr;
    };


  public: // ~~~~ static functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    template<typename T>
    static std::vector<PlotConfig<T>> getPlotConfig();


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    SCReportPopup(const std::string& name, QWidget* parent = nullptr);
    ~SCReportPopup();


  signals: // ~~~~ signals ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void exportRequested(SCReportPopup*);


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    template<typename T>
    void plotGraphs(const T& r) {
      initSummary(*static_cast<const vrfb::shuntcur::ShuntReportData*>(&r));
      for (const auto& p : getPlotConfig<T>()) {
        switch (p.it) {
          case IndexingType::itCell: {
            std::vector<GraphPlotForm::Series> slist {};
            for (std::size_t li = 0; li < r.numLines(); ++li) {
              slist.push_back({"Line " + std::to_string(li+1)});
              for (std::size_t i = 0; i < r.numCells()*r.numStacks(); ++i) {
                slist.back().xs.push_back(i);
                slist.back().ys.push_back((r.*p.supplier)(li*r.numCells()*r.numStacks()+i));
              }
            }
            initPlot(slist, "Cell No.", "Current (A)", p.name);
          }
        }
      }
    }

    bool exportImages(logger::Logger& l);


  private:
    void initSummary(const vrfb::shuntcur::ShuntReportData& r);
    void initPlot(const std::vector<GraphPlotForm::Series>& slist,
        const std::string& name_x, const std::string& name_y,
        const std::string& name);

    void deleteSelf(int) {delete this;}


  private slots:
    void on_exportBtn_clicked();


  private: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    struct Plot {
      std::string name;
      GraphPlotFormTitled* plot = nullptr;
    };


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Ui::SCReportPopup* ui;
    std::string prefix;
    std::vector<Plot> plots;
};
