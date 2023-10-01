#pragma once

#include <string>


namespace logger { // BEGIN OF NAMESPACE <logger> ==============================


enum class Level {
  kFine,
  kInfo,
  kWarn,
  kFail,
  kSucc
};


struct LogMsg {
  Level lvl;
  std::string msg;
};


class Logger {
  public:
    virtual void log(const LogMsg&) = 0;

    inline void fine(const std::string& msg) {
      log({Level::kFine, msg});
    }

    inline void info(const std::string& msg) {
      log({Level::kInfo, msg});
    }

    inline void warn(const std::string& msg) {
      log({Level::kWarn, msg});
    }

    inline void fail(const std::string& msg) {
      log({Level::kFail, msg});
    }

    inline void succ(const std::string& msg) {
      log({Level::kSucc, msg});
    }
};


} // END OF NAMESPACE <logger> -------------------------------------------------
