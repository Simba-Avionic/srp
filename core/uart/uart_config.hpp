/**
 * @file uart_config.hpp
 * @brief Konfiguracja portu szeregowego (termios) wspolna dla wszystkich uzytkownikow core/uart.
 *
 * Presety odtwarzaja 1:1 dawne zachowanie:
 *  - Legacy()  - dawny core/uart (GPS NMEA, RS485/Modbus): tryb kanoniczny, VMIN=0, VTIME=10,
 *  - Mavlink() - dawny radio-driver (radio EC/FC): tryb raw, VMIN=0, VTIME=10.
 *
 * @copyright Copyright (c) 2025
 */
#ifndef CORE_UART_UART_CONFIG_HPP_
#define CORE_UART_UART_CONFIG_HPP_

#include <termios.h>

#include <cstdint>

namespace srp {
namespace core {
namespace uart {

/// Stala termios (np. B57600) dla liczbowej predkosci; 0 gdy predkosc nie jest obslugiwana.
constexpr speed_t GetBaudrateConstant(uint32_t baudrate) {
  switch (baudrate) {
      case 9600:    return B9600;
      case 19200:   return B19200;
      case 38400:   return B38400;
      case 57600:   return B57600;
      case 115200:  return B115200;
      case 230400:  return B230400;
      case 460800:  return B460800;
      case 500000:  return B500000;
      case 576000:  return B576000;
      case 921600:  return B921600;
      case 1000000: return B1000000;
      case 1152000: return B1152000;
      case 1500000: return B1500000;
      case 2000000: return B2000000;
      case 2500000: return B2500000;
      case 3000000: return B3000000;
      case 3500000: return B3500000;
      case 4000000: return B4000000;
      default:      return 0;
  }
}

enum class Parity : uint8_t { kNone, kEven, kOdd };

struct UartConfig {
  speed_t baudrate{B9600};        ///< stala termios (B9600...), nie liczba - patrz GetBaudrateConstant()
  uint8_t data_bits{8};           ///< 5..8
  Parity parity{Parity::kNone};
  uint8_t stop_bits{1};           ///< 1 albo 2
  bool hw_flow_control{false};    ///< CRTSCTS
  bool raw{false};                ///< true: bez ICANON/ISIG/ECHO*/XON-XOFF/obrobki wejscia i wyjscia
  uint8_t vmin{0};
  uint8_t vtime{10};              ///< dziesiate czesci sekundy (przy vmin == 0 timeout odczytu)
  uint16_t default_read_size{256};  ///< Read(0) czyta maksymalnie tyle bajtow
  bool fail_on_apply_error{false};  ///< true: Open() zwraca false gdy tcsetattr() sie nie powiedzie

  /// Dawny core/uart: tryb kanoniczny (liniowy, np. NMEA), brak obrobki flag wejscia/wyjscia/lokalnych.
  static UartConfig Legacy(speed_t baudrate) {
    UartConfig c;
    c.baudrate = baudrate;
    return c;
  }

  /// Dawny radio-driver: tryb raw dla binarnego MAVLinka, predkosc podana liczbowo (np. 57600).
  static UartConfig Mavlink(uint32_t baudrate) {
    UartConfig c;
    c.baudrate = GetBaudrateConstant(baudrate);
    c.raw = true;
    c.default_read_size = 512;
    c.fail_on_apply_error = true;
    return c;
  }

  /// Nanosi konfiguracje na struct termios pobrany wczesniej przez tcgetattr().
  void ApplyTo(struct termios* tty) const {
    cfsetospeed(tty, baudrate);
    cfsetispeed(tty, baudrate);
    if (parity == Parity::kNone) {
      tty->c_cflag &= ~PARENB;  // brak parzystosci
    } else {
      tty->c_cflag |= PARENB;
      if (parity == Parity::kOdd) {
        tty->c_cflag |= PARODD;
      } else {
        tty->c_cflag &= ~PARODD;
      }
    }
    if (stop_bits == 2) {
      tty->c_cflag |= CSTOPB;
    } else {
      tty->c_cflag &= ~CSTOPB;  // jeden bit stopu
    }
    tty->c_cflag &= ~CSIZE;
    switch (data_bits) {
      case 5: tty->c_cflag |= CS5; break;
      case 6: tty->c_cflag |= CS6; break;
      case 7: tty->c_cflag |= CS7; break;
      default: tty->c_cflag |= CS8; break;
    }
    if (hw_flow_control) {
      tty->c_cflag |= CRTSCTS;
    } else {
      tty->c_cflag &= ~CRTSCTS;
    }
    if (raw) {
      tty->c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHONL);
      tty->c_iflag &= ~(IXON | IXOFF | IXANY);
      tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
      tty->c_oflag &= ~(OPOST | ONLCR);
    }
    tty->c_cflag |= CREAD | CLOCAL;
    tty->c_cc[VTIME] = vtime;
    tty->c_cc[VMIN] = vmin;
  }
};

}  // namespace uart
}  // namespace core
}  // namespace srp

#endif  // CORE_UART_UART_CONFIG_HPP_
