#include <fstream>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif
namespace rpo {
namespace utils {
namespace systeminfo {

std::string getHostname() {
  char buffer[256];
#ifdef _WIN32
  if (gethostname(buffer, sizeof(buffer)) == SOCKET_ERROR) {
    return "Error retrieving hostname";
  }
#else
  if (gethostname(buffer, sizeof(buffer)) == -1) {
    return "Error retrieving hostname";
  }
#endif
  return buffer;
}
std::string getUsername() {
#ifdef _WIN32
  const char* username = std::getenv("USERNAME");
#else
  const char* username = std::getenv("USER");
#endif

  if (username == nullptr) {
    return "Error retrieving username";
  }

  return username;
}
// Returns allocated memory of this process in MB.
// On failure, returns 0.0
// on windows -1
double process_mem_usage() {
#ifndef _WIN32

  double resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  std::string pid, comm, state, ppid, pgrp, session, tty_nr;
  std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  std::string utime, stime, cutime, cstime, priority, nice;
  std::string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >>
      minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >>
      nice >> O >> itrealvalue >> starttime >> vsize >> rss;  // don't care about the rest

  stat_stream.close();

  long page_size_kb =
      sysconf(_SC_PAGE_SIZE) / 1024;  // in case x86-64 is configured to use 2MB pages
  resident_set = rss * page_size_kb;
  return resident_set / 1024.0;
#else
  return -1;

#endif
}
double process_mem_usage(pid_t id) {
#ifndef _WIN32

  double resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  std::ifstream stat_stream("/proc/" + std::to_string(id) + "/stat", std::ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  std::string pid, comm, state, ppid, pgrp, session, tty_nr;
  std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  std::string utime, stime, cutime, cstime, priority, nice;
  std::string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >>
      minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >>
      nice >> O >> itrealvalue >> starttime >> vsize >> rss;  // don't care about the rest

  stat_stream.close();

  long page_size_kb =
      sysconf(_SC_PAGE_SIZE) / 1024;  // in case x86-64 is configured to use 2MB pages
  resident_set = rss * page_size_kb;
  return resident_set / 1024.0;
#else
  return -1;

#endif
}
}  // namespace systeminfo
}  // namespace utils
}  // namespace rpo
