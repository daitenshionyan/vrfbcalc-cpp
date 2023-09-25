#pragma once

#include <exception>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


namespace comutils { // ==== <comutils> ========================================


namespace string { // ==== <comutils::string> ==================================


/**
 * Strips away leading and trailing characters of a specified set of characters
 * from a given string.
 *
 * @param str The string to strip.
 * @param chars The set of character to strip away. Defaults to whitespace
 *    characters.
*/
std::string strip(const std::string& str, const std::string& chars = " \t\f\v\n\r");


/**
 * Splits a given string around the specified delimiter character.
 *
 * @param str The string to split.
 * @param delim The delimiter to split at. Defaults to a commar (`,`).
 * @return A vector of substrings obtained by splitting the given string at the
 *    specified delimiter.
*/
std::vector<std::string> split(const std::string& str, char delim = ',');

/**
 * Splits a given string around the specified delimiter characters. Split
 * substrings are pushed back into the given vector of strings.
 *
 * @param str The string to split.
 * @param delim The delimiter to split at. Defaults to a commar (`,`).
*/
void split(const std::string& str, std::vector<std::string>& vec, char delim = ',');


/**
 * Returns a formatted string using the specified format string and arguments.
 *
 * @param format The formatted string.
 * @param args Arguments to be referenced by the formatted string.
 * @throws std::runtime_error if formatting fails.
*/
template<typename ... Args>
std::string format_string(const std::string& format, const Args& ... args) {
  std::size_t count = std::snprintf(nullptr, 0, format.c_str(), args ...);
  if (count < 0) {
    throw std::runtime_error("Error while formatting");
  }
  std::unique_ptr<char[]> buf(new char[count+1]);
  std::snprintf(buf.get(), count+1, format.c_str(), args ...);
  return std::string(buf.get(), count);
}


} // ---- namespace <comutils::string>
// namespace <comutils>


namespace io { // ==== <comutils::io> ==========================================


/**
 * Returns `true` if the specified file name is valid and `false` otherwise.
 *
 * @param name The name to validate.
*/
bool isValidFileName(const std::string& name);


} // ---- <comutils::io>
// namespace <comutils>


namespace time { // ==== <comutils::time> ======================================


/**
 * Returns the time in seconds of a given string timestamp.
 *
 * @param timeStr The string to parse the timestamp from.
 * @param delim The delimiter to separate hours, minutes and seconds.
*/
double parseTimestamp(const std::string& timeStr, char delim = ':');


/**
 * Returns the current local time as a formatted string.
*/
std::string getftime();


} // ---- <comutils::time>
// namespace <comutils>


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

    /**
     * Returns the value of the specified element in the tbble in the specified
     * type `T`.
     *
     * @param <T> The type to get the value as. Defaults to a string.
     * @param h The column header of the element to retrieve.
     * @param r The row number of the element to retrieve.
    */
    template<typename T = std::string>
    T get(const std::string& h, std::size_t r) const;

    /**
     * Returns the value of the specified element in the tbble in the specified
     * type `T`.
     *
     * @param <T> The type to get the value as. Defaults to a string.
     * @param c The column number of the element to retrieve.
     * @param r The row number of the element to retrieve.
    */
    template<typename T = std::string>
    T get(std::size_t c, std::size_t r) const;


  protected:
    inline const std::vector<std::string>& at(const std::string& h) const {
      try {
        return colMap.at(h);
      } catch (std::out_of_range oor) {
        throw std::runtime_error(comutils::string::format_string(
            "Header does not exist '%s'",
            h.c_str()));
      }
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


// ~~~~ io operators ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


std::ostream& operator<<(std::ostream&, const Table&);


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


} // ---- <comutils>
