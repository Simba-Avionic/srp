// Copyright 2025 Jakub Nowak

#ifndef CSVREADER_CSVROW_H_
#define CSVREADER_CSVROW_H_

#include <string>
#include <tuple>

template<typename... Types>
class CsvRow {
 private:
  std::tuple<Types...> data;

  template <std::size_t Index>
  void setValue(const std::string &s);

  template <typename T>
  T convert(const std::string &s);

  template <std::size_t I = 0>
  void setValueRuntime(std::size_t index, const std::string &value);
 public:
  CsvRow(std::string line, char delimiter);

  template <std::size_t I>
  decltype(auto) get() const;
};


#include "CsvRow.tpp"

#endif  // CSVREADER_CSVROW_H_

