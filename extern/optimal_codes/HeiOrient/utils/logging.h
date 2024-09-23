#pragma once
#include <chrono>
#include <iostream>
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
struct AggregateTimer {
  std::chrono::duration<double> _duration{};
  std::string name;
  AggregateTimer(const std::string& n) : name(n) {}
  ~AggregateTimer() { std::cout << name << " took " << _duration.count() << "s" << std::endl; }
};
class PartTimer {
  AggregateTimer& agg;
  std::chrono::high_resolution_clock::time_point start;

 public:
  PartTimer(AggregateTimer& _agg) : agg(_agg) { start = std::chrono::high_resolution_clock::now(); }
  ~PartTimer() {
    agg._duration += std::chrono::duration_cast<std::chrono::duration<double>>(
        std::chrono::high_resolution_clock::now() - start);
  }
};
#define FUNC_NAME __PRETTY_FUNCTION__
static NullStream nullStream;
#ifdef LOGGING_ENABLED_T
#define AGG_TIMER(timer, text) AggregateTimer timer(text);
#define AGG_TIMER_PART(timer) PartTimer inner_timer(timer);
#define TIMED_FUNC(timer) Timer timer(FUNC_NAME);
#define TIMED_SCOPE(timer, text) Timer timer(text);
#define VLOG(vlevel, ...) NullStream()
#define LOG(vlevel) NullStream()
#define LOG_IF(vlevel, ...) NullStream()
#define VLOG_IF(vlevel, ...) NullStream()
#else
#define AGG_TIMER(timer, text)
#define AGG_TIMER_PART(timer)
#define VLOG(vlevel, ...) NullStream()
#define LOG(vlevel) NullStream()
#define TIMED_FUNC(timer) ;
#define TIMED_SCOPE(timer, text)
#define MAKE_LOGGABLE(a, b, c) void terer()
#define LOG_IF(vlevel, ...) NullStream()
#define VLOG_IF(vlevel, ...) NullStream()
#endif

// TODO(https://github.com/rpo/toolkit/issues/1)