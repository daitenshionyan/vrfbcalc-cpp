#pragma once


#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


namespace vrfb {


using ColMap = std::unordered_map<std::string, std::vector<std::string>>;


class Table {
  public:
    using iterator = std::vector<std::string>::const_iterator;

    struct list_view {
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
    Table(const Table&) = default;
    Table(Table&&) = default;

    Table& operator=(Table&) = default;
    Table& operator=(Table&&) = default;

    ~Table() = default;

    /* Returns the number of rows the table has. */
    inline std::size_t numRows() const {return r_size;}

    /* Returns the number of columns the table has. */
    inline std::size_t numCols() const {return c_size;}

    /* Returns list like view of the headers of the table. */
    inline list_view headers() const {return {hdrs.begin(), hdrs.end()};}


    /*
      Returns the value of the cell in the specified location.

      @param <T> Return value type.
      @param h Header of column of cell to return.
      @param r Row number of cell to return.
      @throws std::out_of_range if the specified header does not exist or if
          the specifed row number is out of range.
    */
    template<typename T = std::string>
    T get(const std::string& h, const std::size_t r) const;

    /*
      Returns the value of the cell in the specified location.

      @param <T> Return value type.
      @param c Column number of cell to return.
      @param r Row number of cell to return.
      @throws std::out_of_range if the specified column or row number is out of
          range.
    */
    template<typename T = std::string>
    T get(const std::size_t, const std::size_t r) const;


  protected:
    /*
      Returns a list like view of the column at the specified header.

      @param h Header of column to return.
      @throws std::out_of_range - if the specified header does not exist.
    */
    const std::vector<std::string>& at(const std::string& h) const;

    /*
      Returns a list like view of the column at the specified header.

      @param c Column number of column to return.
      @throws std::out_of_range - if specified column number is out of range.
    */
    inline const std::vector<std::string>& at(const std::size_t c) const {
      return colMap.at(hdrs.at(c));
    }

    /*
      Returns the std::string value of the cell at the specified location.

      @param h Header of column of the cell to return.
      @param r Row number of cell to return.
      @throws std::out_of_range if the specified header does not exist or if
          the specifed row number is out of range.
    */
    inline const std::string& at(const std::string& h, const std::size_t r) const {
      return at(h).at(r);
    }

    /*
      Returns the std::string value of the cell at the specified location.

      @param c Column number of cell to return.
      @param r Row number of cell to return.
      @throws std::out_of_range if the specified column or row number is out of
          range.
    */
    inline const std::string& at(const std::size_t c, const std::size_t r) const  {
      return at(c).at(r);
    }


  private:
    std::size_t c_size;
    std::size_t r_size;
    std::vector<std::string> hdrs;
    ColMap colMap;
};


class invalid_csv_format : public std::exception {
  public:
    invalid_csv_format(const std::string& d) : desc{d} {}

    const char* what() const override {
      return desc.c_str();
    }

  private:
    std::string desc;
};


void readLine_CSV(std::istream&, std::vector<std::string>&);
Table readTable_CSV(std::istream&);

Table readTable_XLXS(std::istream&);

void writeCell_CSV(std::ostream&, const std::string&);
void writeTable_CSV(std::ostream&, const Table&);



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
