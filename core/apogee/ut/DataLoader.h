#ifndef CORE_APOGEE_UT_DATALOADER_H_
#define CORE_APOGEE_UT_DATALOADER_H_

#include <string>
#include <vector>

struct ConfigData {
  double max_height_clean = 0.0;
  double time_to_apogee = 0.0;
  double hz = 0.0;
  bool loaded = false;
};

void checkAndGenerateConfig(const std::string& configPath);

ConfigData readConfig(const std::string& path);

std::vector<double> readColumnCSV(const std::string& path, size_t colIndex);

#endif  