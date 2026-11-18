/**
 * Copyright (c) 2024
 *
 * @file termios_uart.cpp
 */
#include "core/radio/e220/termios_uart.hpp"

#include <fcntl.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>

namespace srp {
namespace core {
namespace radio {
namespace e220 {

namespace {
// tcsetattr() zwraca 0 takze wtedy, gdy sterownik (np. pty) po cichu zrzucil czesc ustawien; nowsze glibc nie
// weryfikuja juz wyniku, wiec porownujemy bity linii (rozmiar znaku, parzystosc, bity stopu) z odczytem zwrotnym.
bool AppliedAsRequested(int fd, int set_rc, const struct termios& wanted) {
  if (set_rc != 0) {
    return false;
  }
  struct termios actual;
  if (::tcgetattr(fd, &actual) != 0) {
    return false;
  }
  constexpr tcflag_t kLineBits = CSIZE | PARENB | PARODD | CSTOPB;
  return (actual.c_cflag & kLineBits) == (wanted.c_cflag & kLineBits);
}
}  // namespace

bool TermiosUart::Open(const std::string& portName, const uint32_t& baudrate, const uint8_t) {
  return Open(portName, uart::UartConfig::Legacy(baudrate));
}

bool TermiosUart::Open(const std::string& portName, const uart::UartConfig& config) {
  if (config.baudrate == B0) {
    return false;  // B0 = zawieszenie; tyle zwraca GetBaudrateConstant() dla nieobslugiwanej predkosci
  }
  Close();
  std::lock_guard<std::mutex> lock{open_mutex_};
  const int fd = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_CLOEXEC);
  if (fd < 0) {
    return false;
  }
  struct termios tty;
  if (::tcgetattr(fd, &tty) != 0) {
    ::close(fd);
    return false;
  }
  config.ApplyTo(&tty);
  const int rc = ::tcsetattr(fd, TCSANOW, &tty);
  if (config.fail_on_apply_error && !AppliedAsRequested(fd, rc, tty)) {
    ::close(fd);
    return false;
  }
  ::tcflush(fd, TCIOFLUSH);
  fd_.store(fd);
  return true;
}

void TermiosUart::Close() {
  std::lock_guard<std::mutex> lock{open_mutex_};
  const int fd = fd_.exchange(-1);
  if (fd >= 0) {
    ::close(fd);
  }
}

bool TermiosUart::WaitForData(std::chrono::milliseconds timeout, const std::stop_token& stoken) {
  if (stoken.stop_requested()) {
    return false;
  }
  const int efd = ::eventfd(0, EFD_CLOEXEC);
  if (efd < 0) {
    return false;
  }
  bool has_data = false;
  {
    const std::stop_callback wake{stoken, [efd]() {
      const std::uint64_t one = 1;
      const ssize_t unused = ::write(efd, &one, sizeof(one));
      static_cast<void>(unused);
    }};
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    for (;;) {
      const int fd = fd_.load();
      if (fd < 0) {
        break;
      }
      const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(
          deadline - std::chrono::steady_clock::now()).count();
      if (left < 0) {
        break;
      }
      struct pollfd fds[2];
      fds[0] = {fd, POLLIN, 0};
      fds[1] = {efd, POLLIN, 0};
      const int ret = ::poll(fds, 2, static_cast<int>(std::min<long long>(left, 1000)));  // NOLINT
      if (ret < 0 && errno == EINTR) {
        continue;
      }
      if (ret < 0 || (fds[1].revents & POLLIN)) {
        break;  // blad albo stop
      }
      if (ret == 0) {
        if (left <= 1000) {
          break;  // timeout
        }
        continue;
      }
      // POLLERR/POLLHUP: Read() zglosi blad (nullopt) i driver zglosi utrate urzadzenia
      has_data = (fds[0].revents & (POLLIN | POLLERR | POLLHUP | POLLNVAL)) != 0;
      break;
    }
  }
  ::close(efd);
  return has_data && !stoken.stop_requested();
}

std::optional<std::vector<uint8_t>> TermiosUart::Read(const uint16_t size) {
  const int fd = fd_.load();
  if (fd < 0) {
    return std::nullopt;
  }
  std::vector<uint8_t> res(size == 0 ? 256 : size);
  const ssize_t n = ::read(fd, res.data(), res.size());
  if (n > 0) {
    res.resize(static_cast<std::size_t>(n));
    return res;
  }
  if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
    return std::vector<uint8_t>{};  // brak danych (poll sie mylil / przerwanie): nie blad
  }
  return std::nullopt;  // EOF (0), EIO (np. zamkniety master pty), utrata urzadzenia
}

core::ErrorCode TermiosUart::Write(const std::vector<uint8_t>& data) {
  std::size_t done = 0;
  while (done < data.size()) {
    const int fd = fd_.load();
    if (fd < 0) {
      return core::ErrorCode::kConnectionError;
    }
    const ssize_t n = ::write(fd, data.data() + done, data.size() - done);
    if (n > 0) {
      done += static_cast<std::size_t>(n);
      continue;
    }
    if (n < 0 && errno == EINTR) {
      continue;
    }
    if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      struct pollfd p {fd, POLLOUT, 0};
      if (::poll(&p, 1, 1000) <= 0) {
        return core::ErrorCode::kError;  // bufor nie zwolnil sie w 1 s
      }
      continue;
    }
    return core::ErrorCode::kConnectionError;
  }
  return core::ErrorCode::kOk;
}

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp
