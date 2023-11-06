#pragma once

#include <string>


namespace comutils {
namespace concurrent {


/**
 * Promise interface to provide a form of communication between caller and
 * concurrent task.
 *
 * @param <T> Task result type.
*/
template<typename T>
class BasePromise {
  public: // ~~~~ constructor / assignment / destructor ~~~~~~~~~~~~~~~~~~~~~~~~
    BasePromise() = default;
    BasePromise(const BasePromise&) = default;
    BasePromise(BasePromise&&) = default;

    BasePromise& operator=(const BasePromise&) = default;
    BasePromise& operator=(BasePromise&&) = default;

    virtual ~BasePromise() = default;


  public: // ~~~~ functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual bool addResult(const T& result, int index=-1) = 0;
    virtual bool addResult(T&& result, int index=-1) = 0;
    virtual void finish() = 0;
    virtual bool isCancelled() const = 0;
    virtual void setProgressRange(int minimum, int maximum) = 0;
    virtual void setProgressValue(int progressValue) = 0;
    virtual void setProgressValueAndText(
          int progressValue, const std::string& progressText) = 0;
    virtual void start() = 0;
    virtual void suspendIfRequested() = 0;
};


}
}
