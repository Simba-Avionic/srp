/**
 * @file ExampleApp.cc
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-08-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "example_adaptive/ExampleApp/code/application/ExampleApp.h"

#include <iostream>

#include "ara/diag/monitor.h"
#include "ara/log/log.h"
#include "core/common/condition.h"
#include "example_adaptive/ExampleApp/code/application/example_did.h"
#include "example_adaptive/ExampleApp/code/application/example_service.h"
#include "srp/example/ExampleDataStructure.h"
#include "srp/example/ExampleService/ExampleServiceHandler.h"
#include "srp/example/ExampleServiceSkeleton.h"

namespace srp {
namespace example {
ExampleApp::ExampleApp() {}
ExampleApp::~ExampleApp() {}

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstring>
#include <cstdint>
#include <vector>

// Adres I2C – zmień w zależności od sprzętu (0x4A lub 0x4B)
#define BNO08X_I2C_ADDRESS 0x4A

// SHTP kanały (dla SH-2 control)
#define SHTP_CHANNEL_CONTROL 2

// Komenda Set Feature – pierwszy bajt payloadu musi być 0xFD
#define SET_FEATURE_CMD 0xFD

// Przykładowe identyfikatory funkcji dla poszczególnych sensorów
#define FEATURE_ID_ACCELEROMETER 0x01
#define FEATURE_ID_GYROSCOPE     0x02
#define FEATURE_ID_MAGNETOMETER  0x12

// Rozmiary payloadów (bez nagłówka) – przykładowe, 17 bajtów zgodnie z rysunkiem 5-1
#define SET_FEATURE_PAYLOAD_SIZE 17

// Całkowity rozmiar wiadomości = nagłówek (4 bajty) + payload
#define SHTP_MESSAGE_SIZE (4 + SET_FEATURE_PAYLOAD_SIZE)

// Czas oczekiwania na wiadomość startową (w µs)
#define STARTUP_WAIT_US 200000

class BNO08X {
public:
    BNO08X(const std::string &i2cBus);
    ~BNO08X();
    
    // Inicjalizacja urządzenia – odczyt startup message i konfiguracja sensorów
    bool initialize();
    
    // Odczyt jednego pakietu wejściowego i parsowanie danych sensorów
    void readSensorReport();
    
private:
    int i2c_fd;
    uint8_t seqNum; // Numer sekwencyjny dla wysyłanych komend
    std::string busPath;
    
    // Metoda pomocnicza: wysyłanie komendy konfigurującej sensor
    // featureID – identyfikator funkcji (np. 0x01 dla akcelerometru)
    // reportInterval – okres raportowania w µs
    // batchInterval – (0, jeśli nie używamy batchingu)
    void sendSetFeatureCommand(uint8_t featureID, uint32_t reportInterval, uint32_t batchInterval);
    
    // Odczyt wiadomości startowej (SHTP advertisement lub tekst w UART-RVC)
    std::string readStartupMessage();
    
    // Pomocnicze metody do odczytu i zapisu przez I²C
    bool writeBytes(const uint8_t* data, size_t length);
    bool readBytes(uint8_t* data, size_t length);
};

BNO08X::BNO08X(const std::string &i2cBus) : busPath(i2cBus), seqNum(0) {
    i2c_fd = open(busPath.c_str(), O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "Błąd otwierania magistrali I2C: " << busPath << std::endl;
        exit(1);
    }
    if (ioctl(i2c_fd, I2C_SLAVE, BNO08X_I2C_ADDRESS) < 0) {
        std::cerr << "Błąd ustawiania adresu I2C." << std::endl;
        exit(1);
    }
}

BNO08X::~BNO08X() {
    close(i2c_fd);
}

bool BNO08X::writeBytes(const uint8_t* data, size_t length) {
    ssize_t ret = write(i2c_fd, data, length);
    return (ret == (ssize_t)length);
}

bool BNO08X::readBytes(uint8_t* data, size_t length) {
    ssize_t ret = read(i2c_fd, data, length);
    return (ret == (ssize_t)length);
}

std::string BNO08X::readStartupMessage() {
    // Próbujemy odczytać do 64 bajtów – może to być tekst (UART-RVC) lub pakiet reklamowy (SHTP)
    char buffer[64] = {0};
    if (read(i2c_fd, buffer, sizeof(buffer)) > 0) {
        return std::string(buffer);
    }
    return "";
}

void BNO08X::sendSetFeatureCommand(uint8_t featureID, uint32_t reportInterval, uint32_t batchInterval) {
    // Payload: 17 bajtów
    // Format: [0] = SET_FEATURE_CMD, [1] = featureID, [2]=feature flags (tu: przykładowo 0x00 lub 0x01 dla włączenia kalibracji),
    // [3-4] Change sensitivity (0x0000), [5-8] reportInterval (little-endian), [9-12] batchInterval, [13-16] sensor-specific config (0)
    uint8_t payload[SET_FEATURE_PAYLOAD_SIZE] = {0};
    payload[0] = SET_FEATURE_CMD;
    payload[1] = featureID;
    // Feature flags – przykładowo ustawiamy 0x01 by włączyć kalibrację (możesz modyfikować)
    payload[2] = 0x01;
    // Change sensitivity (0)
    payload[3] = 0x00;
    payload[4] = 0x00;
    // Report interval (4 bajty, little-endian)
    payload[5] = reportInterval & 0xFF;
    payload[6] = (reportInterval >> 8) & 0xFF;
    payload[7] = (reportInterval >> 16) & 0xFF;
    payload[8] = (reportInterval >> 24) & 0xFF;
    // Batch interval (4 bajty)
    payload[9]  = batchInterval & 0xFF;
    payload[10] = (batchInterval >> 8) & 0xFF;
    payload[11] = (batchInterval >> 16) & 0xFF;
    payload[12] = (batchInterval >> 24) & 0xFF;
    // Sensor-specific configuration – ustawione na 0 (4 bajty)
    payload[13] = 0x00;
    payload[14] = 0x00;
    payload[15] = 0x00;
    payload[16] = 0x00;
    
    // SHTP nagłówek: 4 bajty
    // Całkowita długość = nagłówek + payload
    uint16_t totalLength = SHTP_MESSAGE_SIZE; // 21 bajtów
    uint8_t header[4];
    header[0] = totalLength & 0xFF;               // LSB długości
    header[1] = (totalLength >> 8) & 0x7F;          // MSB (bit 15 = 0 – brak fragmentacji)
    header[2] = SHTP_CHANNEL_CONTROL;             // Kanał 2
    header[3] = seqNum++;                         // Numer sekwencyjny
    
    // Połącz nagłówek i payload
    uint8_t buffer[SHTP_MESSAGE_SIZE];
    memcpy(buffer, header, 4);
    memcpy(buffer + 4, payload, SET_FEATURE_PAYLOAD_SIZE);
    
    // Wyślij wiadomość
    if (!writeBytes(buffer, SHTP_MESSAGE_SIZE)) {
        std::cerr << "Błąd wysyłania Set Feature dla funkcji: 0x" 
                  << std::hex << (int)featureID << std::dec << std::endl;
    } else {
        std::cout << "Skonfigurowano sensor (ID 0x" << std::hex << (int)featureID << std::dec << ")." << std::endl;
    }
}

bool BNO08X::initialize() {
    // Poczekaj na wysłanie wiadomości startowej
    usleep(STARTUP_WAIT_US);
    std::string startup = readStartupMessage();
    if (!startup.empty()) {
        if (startup[0] == '%') {
            std::cout << "Tryb UART-RVC wykryty. Wiadomość startowa:" << std::endl;
            std::cout << startup << std::endl;
        } else {
            std::cout << "Tryb SHTP (non-UART-RVC) wykryty. Odczytany pakiet reklamowy:" << std::endl;
            for (unsigned char c : startup)
                printf("%02X ", c);
            std::cout << std::endl;
        }
    } else {
        std::cout << "Brak wiadomości startowej." << std::endl;
    }
    
    // Konfiguracja sensorów – przykładowe interwały raportowania (w µs)
    // Akcelerometr: 60 ms, żyroskop: 20 ms, magnetometr: 100 ms; brak batchingu (0)
    sendSetFeatureCommand(FEATURE_ID_ACCELEROMETER, 60000, 0);
    sendSetFeatureCommand(FEATURE_ID_GYROSCOPE,     20000, 0);
    sendSetFeatureCommand(FEATURE_ID_MAGNETOMETER,  100000, 0);
    
    return true;
}

void BNO08X::readSensorReport() {
    // Odczyt SHTP nagłówka (4 bajty)
    uint8_t header[4];
    if (!readBytes(header, 4)) {
        std::cerr << "Błąd odczytu nagłówka SHTP." << std::endl;
        return;
    }
    
    // Długość całkowita wiadomości (w tym nagłówek)
    uint16_t totalLength = header[0] | (header[1] << 8);
    if (totalLength < 5) { // przynajmniej jeden bajt payloadu
        std::cerr << "Nieprawidłowa długość wiadomości: " << totalLength << std::endl;
        return;
    }
    uint16_t payloadLength = totalLength - 4;
    std::vector<uint8_t> payload(payloadLength);
    if (!readBytes(payload.data(), payloadLength)) {
        std::cerr << "Błąd odczytu payloadu." << std::endl;
        return;
    }
    
    // Pierwszy bajt payloadu to Report ID
    uint8_t reportID = payload[0];
    switch(reportID) {
        case 0x01: { // Przykładowy raport z akcelerometru
            if (payloadLength < 10) break;
            int16_t accelX = payload[3] << 8 | payload[2];
            int16_t accelY = payload[5] << 8 | payload[4];
            int16_t accelZ = payload[7] << 8 | payload[6];
            std::cout << "Akcelerometr: X=" << accelX 
                      << " Y=" << accelY 
                      << " Z=" << accelZ << std::endl;
            break;
        }
        case 0x02: { // Raport z żyroskopu (zgodnie z rysunkiem 1-34)
            if (payloadLength < 10) break;
            int16_t gyroX = payload[4] << 8 | payload[3];
            int16_t gyroY = payload[6] << 8 | payload[5];
            int16_t gyroZ = payload[8] << 8 | payload[7];
            std::cout << "Żyroskop: X=" << gyroX 
                      << " Y=" << gyroY 
                      << " Z=" << gyroZ << std::endl;
            break;
        }
        case 0x12: { // Przykładowy raport z magnetometru – przyjmujemy, że Report ID = 0x12
            if (payloadLength < 10) break;
            int16_t magX = payload[3] << 8 | payload[2];
            int16_t magY = payload[5] << 8 | payload[4];
            int16_t magZ = payload[7] << 8 | payload[6];
            std::cout << "Magnetometr: X=" << magX 
                      << " Y=" << magY 
                      << " Z=" << magZ << std::endl;
            break;
        }
        default:
            std::cout << "Nieznany Report ID: 0x" << std::hex << (int)reportID << std::dec << std::endl;
            break;
    }
}


int ExampleApp::Initialize(
    const std::map<ara::core::StringView, ara::core::StringView> parms) {
  ara::log::LogInfo() << "App Initialized";
  return 0;
}

int ExampleApp::Run(const std::stop_token& token) {
  BNO08X sensor("/dev/i2c-1");
  if (!sensor.initialize()) {
      std::cerr << "Inicjalizacja BNO08X nie powiodła się." << std::endl;
      return 1;
  }
  
  // Przykładowa pętla odczytu danych sensorów
  while (!token.stop_requested()) {
      sensor.readSensorReport();
      core::condition::wait_for(std::chrono::milliseconds(100),token);
  }
  return 0;
}

}  // namespace example
}  // namespace srp
