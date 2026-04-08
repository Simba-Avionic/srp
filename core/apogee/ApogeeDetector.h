/**
 * @file ApogeeDetector.h
 * @author Mikita Hapankou
 * @brief 
 * @version 0.1
 * @date 2026-04-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CORE_APOGEE_APOGEEDETECTOR_H_
#define CORE_APOGEE_APOGEEDETECTOR_H_

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
constexpr int ksmallestHeight = -1e9;
class RealTimeApogee {
 public:
  // bufferSize - liczba ostatnich pomiarów używanych do obliczenia średniej prędkości pionowej
  // speedThreshold - próg prędkości pionowej (ujemna liczba!!); apogeum uznajemy za
  // osiągnięte, gdy średnia prędkość spadnie poniżej tego progu
  // startHeight - wysokość startowa (domyślnie 0)
  explicit RealTimeApogee(size_t bufferSize = 15, double speedThreshold = -0.5,
                          double startHeight = 0)
      : bufferSize(bufferSize),
        speedThreshold(speedThreshold),
        maxHeight(ksmallestHeight),
        startHeight(startHeight),
        detectedApogee(0),
        apogeeReached(false),
        isLaunched(false) {}

  double averageSpeed() const {
    double sum = 0;
    for (double v : speedBuffer) sum += v;
    return sum / speedBuffer.size();
  }

  // funkcja do aktualizacji stanu
  void update(double height, double velocity) {
    height = height - startHeight;
    if (!isLaunched && velocity > 5.0) {
      isLaunched = true;
    }

    if (!isLaunched) return;

    heightBuffer.push_back(height);
    speedBuffer.push_back(velocity);

    if (heightBuffer.size() > bufferSize) {
      heightBuffer.erase(heightBuffer.begin());
      speedBuffer.erase(speedBuffer.begin());
    }

    if (height > maxHeight) {
      maxHeight = height;
    }
    if (!apogeeReached && heightBuffer.size() >= bufferSize &&
        averageSpeed() <= speedThreshold) {
      apogeeReached = true;
      detectedApogee = maxHeight;
    }
  }

  bool isApogeeReached() const { return apogeeReached; }
  double getApogee() const { return detectedApogee; }

 private:
  size_t bufferSize;
  double speedThreshold;
  std::vector<double> heightBuffer;
  std::vector<double> speedBuffer;
  double maxHeight;
  double startHeight;
  double detectedApogee;
  bool apogeeReached;
  bool isLaunched;
};

#endif  // CORE_APOGEE_APOGEEDETECTOR_H_
