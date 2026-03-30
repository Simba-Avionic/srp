// Copyright 2025 Jakub Nowak

#ifndef CSVREADER_CSVREADER_H_
#define CSVREADER_CSVREADER_H_

#include <fstream>
#include <string>
#include <vector>

#include "CsvRow.h"

/*
Klasa napisana tylko w celu wygodniejszego odczytu pliku csv.
Poczas tworzenia nie były brane względy wydajnościowe
*/
template <typename... Types>
class CsvReader {
 private:
  std::string filename;
  std::ifstream fileStream;
  bool hasHeader;
  std::vector<CsvRow<Types...>> rows;
  char delimeter;

 public:
  CsvReader(const std::string& filename, bool hasHeader = true,
            char delimeter = ',');
  const CsvRow<Types...>& operator[](std::size_t index) const;
  int rowCount() const;
};

#include "CsvReader.tpp"

#endif  //  CSVREADER_CSVREADER_H_
