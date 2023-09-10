#pragma once


#include <string>
#include <unordered_set>

#include "table.hpp"


namespace vrfb {


/* Structure containing cell efficiency calculation configuration. */
struct Config_CE {
  std::string t_time_h;                           /* Total time header */
  std::string type_h;                             /* Step type header */
  std::string c_capacity_h;                       /* Charging capacity header */
  std::string d_capacity_h;                       /* Dicharging capacity header */
  std::string c_energy_h;                         /* Charging energy header */
  std::string d_energy_h;                         /* Discharging energy header */

  std::unordered_set<std::string> c_type_names;   /* Charging type names */
  std::unordered_set<std::string> d_type_names;   /* Discharging type names */
};


struct Data_CE {
  const Table table;
  const Config_CE cfg;
};


/*
  Calculates the cell efficiency performance of a cell.

  @param datas List of data to process.
*/
vrfb::Table calcPerf_CE(const double area, const std::vector<Data_CE>& datas);


}
