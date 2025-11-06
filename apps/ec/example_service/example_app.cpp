/**
 * @file engine_app.cpp
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "apps/ec/example_service/example_app.hpp"
#include "core/common/condition.h"
#include "ara/log/log.h"
#include <sys/resource.h>
#include <unistd.h>


namespace srp {
namespace apps {


void myFunction() {
    // Simulate CPU and memory usage
    for (volatile int i = 0; i < 100000000; ++i);
    int* arr = new int[10000000];
    delete[] arr;
}



ExampleApp::ExampleApp() {
}

int ExampleApp::Run(const std::stop_token& token) {
  ara::log::LogInfo() << "Running Example Application";
  struct rusage usage_start, usage_end;

  getrusage(RUSAGE_SELF, &usage_start);
  myFunction();
  getrusage(RUSAGE_SELF, &usage_end);

  double user_time =
      (usage_end.ru_utime.tv_sec - usage_start.ru_utime.tv_sec) +
      (usage_end.ru_utime.tv_usec - usage_start.ru_utime.tv_usec) / 1e6;

  double sys_time =
      (usage_end.ru_stime.tv_sec - usage_start.ru_stime.tv_sec) +
      (usage_end.ru_stime.tv_usec - usage_start.ru_stime.tv_usec) / 1e6;

  long mem_usage_kb = usage_end.ru_maxrss - usage_start.ru_maxrss;

  ara::log::LogInfo() << "User CPU time: " << std::to_string(user_time) << " s";
  ara::log::LogInfo() << "System CPU time: " << std::to_string(sys_time) << " s";
  ara::log::LogInfo() << "Memory usage change: " << std::to_string(mem_usage_kb) << " KB";
  core::condition::wait(token);
  ara::log::LogInfo() << "Run complete, closing";
  return 0;
}

int ExampleApp::Initialize(const std::map<ara::core::StringView, ara::core::StringView>
                      parms) {
                        
  ara::log::LogInfo() << "Initialize Complete";
  return 0;
}

}  // namespace apps
}  // namespace srp
