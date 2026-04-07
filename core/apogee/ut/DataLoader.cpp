// Copyright 2025 Mikita Hapankou
#include "DataLoader.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "csvReader/CsvReader.h"

// Funkcja pomocnicza do usuwania spacji i znaków nowej linii z brzegów stringa
std::string trim(const std::string& str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (std::string::npos == first) return "";
  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

// Funkcja pomocnicza do wyciągnięcia ścieżki do flight_truth.csv na podstawie ścieżki do config.txt
std::string getTruthPath(const std::string& configPath) {
  size_t lastSlash = configPath.find_last_of("/\\");
  if (lastSlash != std::string::npos) {
    return configPath.substr(0, lastSlash + 1) + "flight_truth.csv";
  }
  return "flight_truth.csv";
}

void checkAndGenerateConfig(const std::string& configPath) {
  std::string truthPath = getTruthPath(configPath);
  std::ifstream truthFile(truthPath);

  // 1. Sprawdzamy czy istnieje flight_truth.csv
  if (truthFile.is_open()) {
    std::cout << ">>> Znaleziono plik referencyjny: " << truthPath << ". Nadpisuje config...\n";

    std::string line;
    // Pomiń nagłówek (t,x,y,z,vx,vy,vz,ax,ay,az)
    std::getline(truthFile, line);

    double max_z = -std::numeric_limits<double>::infinity();
    double time_at_apogee = 0.0;
    double t0 = -1.0, t1 = -1.0;

    // 2. Analiza danych z pliku prawdy
    while (std::getline(truthFile, line)) {
      std::stringstream ss(line);
      std::string cell;
      std::vector<double> row;

      while (std::getline(ss, cell, ',')) {
        try {
          row.push_back(std::stod(cell));
        } catch (...) {
          row.push_back(0.0);
        }
      }

      if (row.size() >= 4) {
        double t = row[0];
        double z = row[3];

        if (t0 < 0) {
          t0 = t;
        } else if (t1 < 0) {
          t1 = t;
        }

        if (z > max_z) {
          max_z = z;
          time_at_apogee = t;
        }
      }
    }

    // Wyliczenie częstotliwości (hz)
    double hz = 100.0;
    if (t0 >= 0 && t1 > t0) {
      hz = 1.0 / (t1 - t0);
    }

    // 3. Nadpisanie pliku config.txt
    std::ofstream outConfig(configPath);
    if (outConfig.is_open()) {
      outConfig << "max_height=" << max_z << "\n";
      outConfig << "time_to_apogee=" << time_at_apogee << "\n";
      outConfig << "hz=" << hz << "\n";
      std::cout << ">>> Pomyslnie zaktualizowano: " << configPath << "\n";
    }
  } else {
    // 4. Jeśli nie ma flight_truth.csv, polegamy na tym, co wpisał użytkownik
    std::cout << ">>> Brak " << truthPath << ". Uzywam istniejacego " << configPath << "\n";
  }
}

ConfigData readConfig(const std::string& path) {
  ConfigData config;
  config.hz = 10.0;  // Ustawienie domyślnego hz, w razie jakby config był pusty

  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Ostrzezenie: Nie udalo sie otworzyc pliku: " << path << std::endl;
    return config;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;

    auto delimiterPos = line.find('=');
    if (delimiterPos != std::string::npos) {
      std::string key = trim(line.substr(0, delimiterPos));
      std::string valueStr = trim(line.substr(delimiterPos + 1));

      try {
        if (key == "max_height") {
          config.max_height_clean = std::stod(valueStr);
        } else if (key == "time_to_apogee") {
          config.time_to_apogee = std::stod(valueStr);
        } else if (key == "hz") {
          config.hz = std::stod(valueStr);
        }
      } catch (const std::exception& e) {
        std::cerr << "Blad parsowania dla klucza '" << key << "': " << e.what() << std::endl;
      }
    }
  }
  config.loaded = true;
  return config;
}

std::vector<double> readColumnCSV(const std::string& path, size_t colIndex) {
  std::vector<double> data;

  try {
    if (path.find("accel") != std::string::npos) {
      CsvReader<double, double, double, double> csvReader(path);
      for (int i = 0; i < csvReader.rowCount(); ++i) {
        auto row = csvReader[i];
        switch (colIndex) {
          case 0:
            data.push_back(row.template get<0>());
            break;
          case 1:
            data.push_back(row.template get<1>());
            break;
          case 2:
            data.push_back(row.template get<2>());
            break;
          case 3:
            data.push_back(row.template get<3>());
            break;
          default:
            break;
        }
      }
    } else if (path.find("baromet") != std::string::npos) {
      CsvReader<double, double> csvReader(path);
      for (int i = 0; i < csvReader.rowCount(); ++i) {
        auto row = csvReader[i];
        switch (colIndex) {
          case 0:
            data.push_back(row.template get<0>());
            break;
          case 1:
            data.push_back(row.template get<1>());
            break;
          default:
            break;
        }
      }
    } else {
      std::cerr << "Ostrzezenie: Typ pliku nierozpoznany po sciezce: " << path << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error reading CSV file " << path << ": " << e.what() << std::endl;
  }

  return data;
}
