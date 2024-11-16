/**
 * @file csvdriver.cc
 * @author Michał Mańkowski (m.mankowski2004@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "core/csvdriver/csvdriver.h"
#include <string>
namespace simba {
namespace csv {

CSVDriver::CSVDriver(const char& separator): separator_(separator) {
}
void CSVDriver::Close() {
    csvFile.close();
}
int CSVDriver::Open(const std::string& fileName, const std::string& HEADER) {
    this->csvFile = std::ofstream(fileName, std::ios::app);
    if (!csvFile.is_open()) {
        return -1;
    }
    csvFile << HEADER << "\n";
    return 0;
}

int CSVDriver::WriteLine(const std::string& line) {
  csvFile << line << "\n";
  csvFile.flush();
  return 0;
}

}  // namespace csv
}  // namespace simba
