/**
 * @file main.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "apps/example/router.h"
#include "core/application/application_factory.h"
#include "untar/untar.h"

int main(int argc, char const *argv[]) {
  simba::core::ApplicationFactory<simba::router::Router>::Start(argc, argv);
  return 0;
}

// #include <sys/sysinfo.h>

// #include <chrono>
// #include <iostream>

// int main(int argc, char const *argv[]) {
//   std::chrono::milliseconds uptime(0u);
//   struct sysinfo x;
//   while (true) {
//     if (sysinfo(&x) == 0) {
//       uptime = std::chrono::milliseconds(
//           static_cast<unsigned long long>(x.uptime) * 1000ULL

//       );
//       std::cout << static_cast<uint32_t>(uptime.count()) << std::endl;
//     }
//   }
//   return 0;
// }
