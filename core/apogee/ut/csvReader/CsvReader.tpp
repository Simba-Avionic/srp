/**
 * @file CsvReader.tpp
 * @author Jakub Nowak 
 * @brief 
 * @version 0.1
 * @date 2026-04-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "CsvReader.h"

#include <iostream>
#include <string>
#include <tuple>

template <typename... Types>
CsvReader<Types...>::CsvReader(const std::string& filename, bool hasHeader,
                               char delimeter)
    : fileStream(filename), hasHeader(hasHeader), delimeter(delimeter) {
  if (!fileStream.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  std::string line;
  bool skippedHeader = !hasHeader;
  while (std::getline(fileStream, line)) {
    if (!skippedHeader) {
      skippedHeader = true;
      continue;  // Skip header line
    }
    CsvRow<Types...> row(line, delimeter);
    rows.push_back(row);
  }

  fileStream.close();
}

template <typename... Types>
const CsvRow<Types...>& CsvReader<Types...>::operator[](std::size_t index) const {
  return rows[index];
}

template <typename... Types>
int CsvReader<Types...>::rowCount() const {
  return rows.size();
}
