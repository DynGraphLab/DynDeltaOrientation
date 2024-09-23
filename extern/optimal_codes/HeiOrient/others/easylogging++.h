#pragma once
#include <chrono>
#include <ostream>
class NullStream : public std::ostream {
 public:
  NullStream() : std::ostream(nullptr) {}
};
class Timer {
  std::chrono::high_resolution_clock::time_point start;
  std::string name;

 public:
  Timer(std::string _name) : name(_name) { start = std::chrono::high_resolution_clock::now(); }
  ~Timer() {
    std::cout << name << " took "
              << std::chrono::duration_cast<std::chrono::duration<double>>(
                     std::chrono::high_resolution_clock::now() - start)
                     .count()
              << "s" << std::endl;
  }
};
#define FUNC_NAME __PRETTY_FUNCTION__
static NullStream nullStream;
#define VLOG(vlevel, ...) NullStream()
#define LOG(vlevel) NullStream()
#define TIMED_FUNC(timer) Timer timer(FUNC_NAME);
#define TIMED_SCOPE(timer, text)
#define MAKE_LOGGABLE(a, b, c) void terer()
#define LOG_IF(vlevel, ...) NullStream()
#define VLOG_IF(vlevel, ...) NullStream()

#define assert(X)