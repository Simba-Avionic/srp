/**
 * @file csv_driver_test.cc
 * @author Mateusz Krajewski (matikrajek42@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "core/csvdriver/csvdriver.h"

using namespace simba::csv;  // NOLINT

class CSVDriverTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Ustawienia przed każdym testem - można np. ustawić tymczasowy plik
    testFile = "test_output.csv";
  }

  void TearDown() override {
    // Sprzątanie po każdym teście - usuwanie pliku testowego
    std::remove(testFile.c_str());
  }

  std::string testFile;
};

TEST_F(CSVDriverTest, OpenAndWriteTest) {
  CSVDriver driver(';');

  // Sprawdzenie, czy plik zostanie otwarty poprawnie
  int result = driver.Open(testFile, "Header1;Header2;Header3");
  EXPECT_EQ(result, 0);  // Sprawdzamy, czy otwarcie pliku zakończyło się powodzeniem

  // Sprawdzenie, czy linia zostanie poprawnie zapisana
  result = driver.WriteLine("Value1;Value2;Value3");
  EXPECT_EQ(result, 0);  // Sprawdzamy, czy zapisanie linii zakończyło się powodzeniem

  // Sprawdzamy, czy plik zawiera zapisane dane
  std::ifstream inputFile(testFile);
  std::string line;
  std::getline(inputFile, line);  // Odczytujemy pierwszy wiersz (nagłówek)
  EXPECT_EQ(line, "Header1;Header2;Header3");

  std::getline(inputFile, line);  // Odczytujemy drugi wiersz (dane)
  EXPECT_EQ(line, "Value1;Value2;Value3");

  inputFile.close();
}

TEST_F(CSVDriverTest, OpenFailureTest) {
  CSVDriver driver(';');
  int result = driver.Open("/invalid/path/test_output.csv", "Header1;Header2;Header3");
  EXPECT_EQ(result, -1);
}

TEST_F(CSVDriverTest, CloseTest) {
  CSVDriver driver(';');
  int result = driver.Open(testFile, "Header1;Header2;Header3");
  EXPECT_EQ(result, 0);
  driver.Close();

  std::ifstream inputFile(testFile);
  ASSERT_TRUE(inputFile.is_open());
  inputFile.close();
}
