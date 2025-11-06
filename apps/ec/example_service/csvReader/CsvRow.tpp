// Copyright 2025 Jakub Nowak

#include "CsvRow.h"
#include <sstream>
#include <string>
#include <vector>

template <typename... Types>
CsvRow<Types...>::CsvRow(std::string line, char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(line);
  std::string item;

  int i = 0;
  while (std::getline(ss, item, delimiter)) {
    setValueRuntime(i++, item);
  }
}

template <typename... Types>
template <std::size_t Index>
void CsvRow<Types...>::setValue(const std::string &s) {
  using T = typename std::tuple_element<Index, std::tuple<Types...>>::type;
  std::get<Index>(data) = convert<T>(s);
}

template <typename... Types>
template <typename T>
T CsvRow<Types...>::convert(const std::string &s) {
  std::istringstream iss(s);
  T val;
  if (!(iss >> val)) {
    throw std::runtime_error("Conversion failed");
  }
  return val;
}

template <typename... Types>
template <std::size_t I>
void CsvRow<Types...>::setValueRuntime(std::size_t index, const std::string &value) {
  if constexpr (I < std::tuple_size_v<decltype(data)>) {
    if (I == index) {
      setValue<I>(value);
    } else {
      setValueRuntime<I + 1>(index, value);
    }
  } else {
    throw std::runtime_error("Index out of bounds");
  }
}

template <typename... Types>
template <std::size_t I>
decltype(auto) CsvRow<Types...>::get() const {
  return std::get<I>(data);
}
