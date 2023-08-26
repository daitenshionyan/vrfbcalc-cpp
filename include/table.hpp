#pragma once


#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


namespace vrfb_utils {


using ColMap = std::unordered_map<std::string, std::vector<std::string>>;


class Table {
 public:
  using iterator = std::vector<std::string>::const_iterator;

  struct header_view {
    iterator b;
    iterator e;

    inline iterator begin() const {
      return b;
    }

    inline iterator end() const {
      return e;
    }

    inline std::string operator[](std::size_t i) {
      return b[i];
    }
  };

  Table(const std::vector<std::string>& h, const std::vector<std::string>& elems);

  Table() = default;
  Table(Table&) = default;
  Table(Table&&) = default;

  Table& operator=(Table&) = default;
  Table& operator=(Table&&) = default;

  ~Table() = default;

  inline std::size_t numRows() const {return r_size;}
  inline std::size_t numCols() const {return c_size;}
  inline header_view headers() const {return {hdrs.begin(), hdrs.end()};}

  inline const std::vector<std::string>& at(const std::string& h) const {
    return colMap.at(h);
  }
  inline const std::vector<std::string>& at(const std::size_t c) const {
    return colMap.at(hdrs.at(c));
  }
  inline const std::string& at(const std::string& h, const std::size_t r) const {
    return at(h).at(r);
  }
  inline const std::string& at(const std::size_t c, const std::size_t r) const  {
    return at(c).at(r);
  }

  template<typename T>
  T get(const std::string& h, const std::size_t r) const;
  template<typename T>
  T get(const std::size_t, const std::size_t r) const;


 private:
  std::size_t c_size;
  std::size_t r_size;
  std::vector<std::string> hdrs;
  ColMap colMap;
};


std::istream& operator>>(std::istream&, Table&);
std::ostream& operator<<(std::ostream&, const Table&);



// ---- get<std::string> -------------------------------------------------------


template<>
inline std::string Table::get<std::string>(const std::string& h, const std::size_t r) const {
  return at(h, r);
}


template<>
inline std::string Table::get<std::string>(const std::size_t c, const std::size_t r) const {
  return at(c, r);
}


// ---- get<int> ---------------------------------------------------------------


template<>
inline int Table::get<int>(const std::string& h, const std::size_t r) const {
  return std::stoi(at(h, r));
}


template<>
inline int Table::get<int>(const std::size_t c, const std::size_t r) const {
  return std::stoi(at(c, r));
}


// ---- get<double> ------------------------------------------------------------


template<>
inline double Table::get<double>(const std::string& h, const std::size_t r) const {
  return std::stod(at(h, r));
}


template<>
inline double Table::get<double>(const std::size_t c, const std::size_t r) const {
  return std::stod(at(c, r));
}


}
