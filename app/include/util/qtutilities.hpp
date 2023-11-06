#pragma once

#include <QException>
#include <QPromise>

#include "utillib/concur.hpp"


namespace vrfbutils {


/**
 * Implementation of `BasePromise<T>` that encapsulates a `QPromise<T>`.
 * The responsibility of deletion the encapsulated `QPromise` is NOT handed over.
 *
 * @param <T> Task return type.
*/
template<typename T>
class BasePromise_Qt : public comutils::concurrent::BasePromise<T> {
  public: // ~~~~ constructor / assignment / destructor ~~~~
    BasePromise_Qt(QPromise<T>* p) : prom_p{p} {}

    BasePromise_Qt() = delete;
    BasePromise_Qt(const BasePromise_Qt&) = delete;
    BasePromise_Qt(BasePromise_Qt&&) = default;

    BasePromise_Qt& operator=(const BasePromise_Qt&) = delete;
    BasePromise_Qt& operator=(BasePromise_Qt&&) = default;

    ~BasePromise_Qt() = default;


  public: // ~~~~ functions ~~~~
    bool addResult(const T& result, int index=-1) override {
      return prom_p->addResult(result, index);
    }

    bool addResult(T&& result, int index=-1) override {
      return prom_p->addResult(std::move(result), index);
    }

    void finish() override {
      prom_p->finish();
    }

    bool isCancelled() const override {
      return prom_p->isCanceled();
    }

    void setProgressRange(int minimum, int maximum) override {
      prom_p->setProgressRange(minimum, maximum);
    }

    void setProgressValue(int progressValue) override {
      prom_p->setProgressValue(progressValue);
    }

    void setProgressValueAndText(int progressValue, const std::string& progressText) override {
      prom_p->setProgressValueAndText(
          progressValue,
          QString::fromStdString(progressText));
    }

    void start() override {
      prom_p->start();
    }

    void suspendIfRequested() override {
      prom_p->suspendIfRequested();
    }


  private:
    /** Encapsulated `QPromise` pointer. DO NOT DELETE!! */
    QPromise<T>* prom_p;
};







class StdQException : public QException {
  public:
    StdQException(const std::exception& ex) : e(ex) {}

    void raise() const override {throw *this;}
    QException* clone() const override {return new StdQException(*this);}

    const std::exception& error() const {return e;}


  private:
    std::exception e;
};


}
