#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>


namespace comutils {


// :::: [ Table ] ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


class Table {
  public: // ~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    using ColMap = std::unordered_map<std::string, std::vector<std::string>>;
    using iterator = std::vector<std::string>::const_iterator;

    class ListView {
      public:
        ListView(const iterator& bb, const iterator& ee)
            : b{bb}, e{ee} {}

        inline iterator begin() const {
          return b;
        }

        inline iterator end() const {
          return e;
        }

        inline const std::string& operator[](std::size_t i) {
          return b[i];
        }


      private:
        iterator b;
        iterator e;
    };


  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    Table(const std::vector<std::string>& h, const std::vector<std::string>& elems);
    Table(std::vector<std::string>&& h, ColMap&& cm);

    Table() = default;
    Table(const Table&) = default;
    Table(Table&&) = default;

    Table& operator=(const Table&) = default;
    Table& operator=(Table&&) = default;

    ~Table() = default;


  public: // ~~~~ accessors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    inline std::size_t numRows() const {return r_size;}
    inline std::size_t numCols() const {return hdrs.size();}
    inline ListView headers() const {return {hdrs.begin(), hdrs.end()};}

    template<typename T = std::string>
    T get(const std::string& h, std::size_t r) const;

    template<typename T = std::string>
    T get(std::size_t c, std::size_t r) const;


  protected:
    inline const std::vector<std::string>& at(const std::string& h) const {
      return colMap.at(h);
    }

    inline const std::vector<std::string>& at(std::size_t c) const {
      return colMap.at(hdrs.at(c));
    }

    inline const std::string& at(const std::string& h, std::size_t r) const {
      return at(h).at(r);
    }

    inline const std::string& at(std::size_t c, std::size_t r) const {
      return at(c).at(r);
    }


  private: // ~~~~ fields ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::size_t r_size;
    std::vector<std::string> hdrs;
    ColMap colMap;
};


// ~~~~ template definition ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~ get<std::string> ~~~~


template<>
inline std::string Table::get<std::string>(const std::string& h, std::size_t r) const {
  return at(h, r);
}


template<>
inline std::string Table::get<std::string>(std::size_t c, std::size_t r) const {
  return at(c, r);
}


// ~~~~ get<int> ~~~~


template<>
inline int Table::get<int>(const std::string& h, std::size_t r) const {
  return std::stoi(at(h, r));
}


template<>
inline int Table::get<int>(std::size_t c, std::size_t r) const {
  return std::stoi(at(c, r));
}


// ~~~~ get<double> ~~~~


template<>
inline double Table::get<double>(const std::string& h, std::size_t r) const {
  return std::stod(at(h, r));
}


template<>
inline double Table::get<double>(std::size_t c, std::size_t r) const {
  return std::stod(at(c, r));
}


}
