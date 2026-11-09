/**
 * @file sim_uart.hpp
 * @brief Wspolny model portu szeregowego dla driverow sim (core/uart, takze radio).
 *
 * Open dziala na dowolnej sciezce (np. /dev/ttyS1, ktorego na gosciu QEMU nie ma). Bajty do odczytu
 * wstrzykuje test przez ctl (kolejka FIFO per port), kazdy Write trafia do events.jsonl i do
 * state/uart/<port>.tx. Polecenia ctl uart: patrz core/sim/README.md.
 *
 * @copyright Copyright (c) 2025
 */
#ifndef CORE_SIM_SIM_UART_HPP_
#define CORE_SIM_SIM_UART_HPP_

#include <chrono>  // NOLINT
#include <cstdint>
#include <deque>
#include <fstream>
#include <iterator>
#include <map>
#include <mutex>  // NOLINT
#include <optional>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "core/sim/sim_ctl.hpp"

namespace srp {
namespace core {
namespace sim {

class UartSim {
 public:
  static UartSim& Get() {
    static UartSim u;
    return u;
  }

  /// Otwiera port; zwraca uchwyt (>= 1000, nie jest deskryptorem pliku) albo -1 przy usterce "fault open".
  /// timeout_ds - jak w prawdziwym drivera (dziesiate czesci sekundy), 0 = domyslne 10 (1 s).
  int Open(const std::string& port, uint8_t timeout_ds) {
    std::lock_guard<std::mutex> l(m_);
    bool fault = false;
    bool tmo = false;
    Sync(port, Op::kOpen, &fault, &tmo);
    if (fault) {
      return -1;
    }
    const int h = next_handle_++;
    handles_[h] = port;
    ports_[port].timeout_ms = (timeout_ds == 0 ? 10 : timeout_ds) * 100;
    Event("uart", "\"op\":\"open\",\"port\":\"" + port + "\"");
    return h;
  }

  void Close(int handle) {
    std::lock_guard<std::mutex> l(m_);
    auto it = handles_.find(handle);
    if (it != handles_.end()) {
      Event("uart", "\"op\":\"close\",\"port\":\"" + it->second + "\"");
      handles_.erase(it);
    }
  }

  /// Jak read() z VMIN=0/VTIME: zwraca dane gdy sa w kolejce, w przeciwnym razie czeka do timeoutu
  /// z Open i zwraca nullopt (bez petli aktywnej - spi). max_size musi byc > 0.
  std::optional<std::vector<uint8_t>> Read(int handle, size_t max_size) {
    std::string port;
    int64_t timeout_ms = 0;
    if (!PortOf(handle, &port, &timeout_ms)) {
      return std::nullopt;
    }
    const int64_t end = SteadyMs() + timeout_ms;
    while (true) {
      {
        std::lock_guard<std::mutex> l(m_);
        bool fault = false;
        bool tmo = false;
        Sync(port, Op::kRead, &fault, &tmo);
        if (fault) {
          return std::nullopt;
        }
        auto& rx = ports_[port].rx;
        if (!tmo && !rx.empty()) {
          const size_t n = rx.size() < max_size ? rx.size() : max_size;
          std::vector<uint8_t> out(rx.begin(), rx.begin() + n);
          rx.erase(rx.begin(), rx.begin() + n);
          Event("uart", "\"op\":\"read\",\"port\":\"" + port + "\",\"hex\":\"" + Hex(out) + "\"");
          return out;
        }
      }
      if (SteadyMs() >= end) {
        return std::nullopt;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }

  /// Jak poll(POLLIN): true gdy w ciagu timeout_ms pojawia sie dane do odczytu.
  bool WaitForData(int handle, int timeout_ms) {
    std::string port;
    int64_t unused = 0;
    if (!PortOf(handle, &port, &unused)) {
      return false;
    }
    const int64_t end = SteadyMs() + timeout_ms;
    while (true) {
      {
        std::lock_guard<std::mutex> l(m_);
        bool fault = false;
        bool tmo = false;
        Sync(port, Op::kPoll, &fault, &tmo);
        if (!tmo && !ports_[port].rx.empty()) {
          return true;
        }
      }
      if (SteadyMs() >= end) {
        return false;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  }

  /// true = zapisano; false = usterka "fault write" albo port nie jest otwarty.
  bool Write(int handle, const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> l(m_);
    auto it = handles_.find(handle);
    if (it == handles_.end()) {
      return false;
    }
    const std::string port = it->second;
    bool fault = false;
    bool tmo = false;
    Sync(port, Op::kWrite, &fault, &tmo);
    if (fault) {
      return false;
    }
    const std::string hex = Hex(data);
    Event("uart", "\"op\":\"write\",\"port\":\"" + port + "\",\"hex\":\"" + hex + "\"");
    const std::string full = Dir() + "/state/uart/" + FileName(port) + ".tx";
    MkDirs(full.substr(0, full.rfind('/')));
    std::ofstream f(full, std::ios::app);
    f << hex << "\n";
    return true;
  }

 private:
  enum class Op { kOpen, kRead, kWrite, kPoll };

  struct Port {
    std::deque<uint8_t> rx;
    int64_t timeout_ms{1000};
  };

  /// "/dev/ttyS1" -> "dev_ttyS1"
  static std::string FileName(const std::string& port) {
    std::string s = port;
    while (!s.empty() && s[0] == '/') {
      s.erase(0, 1);
    }
    for (auto& c : s) {
      if (c == '/') {
        c = '_';
      }
    }
    return s;
  }

  bool PortOf(int handle, std::string* port, int64_t* timeout_ms) {
    std::lock_guard<std::mutex> l(m_);
    auto it = handles_.find(handle);
    if (it == handles_.end()) {
      return false;
    }
    *port = it->second;
    *timeout_ms = ports_[it->second].timeout_ms;
    return true;
  }

  // Stosuje polecenia ctl (wywolywac pod m_). Ustawia usterke dla operacji i flage "timeout" portu.
  void Sync(const std::string& port, Op op, bool* fault, bool* timeout) {
    Ctl::Get().Visit([&](Cmd& c) {
      if (c.tok.empty() || c.tok[0] != "uart") {
        return;
      }
      try {
        if (c.tok.size() >= 2 && c.tok[1] == "reset") {
          if (!c.applied) {
            c.applied = true;
            for (auto& p : ports_) {
              p.second.rx.clear();
            }
            Event("uart", "\"op\":\"reset\"");
          }
          return;
        }
        if (c.tok.size() < 3) {
          return;
        }
        const std::string& p = c.tok[1];
        const std::string& what = c.tok[2];
        if (what == "rx" && !c.applied) {
          c.applied = true;
          const auto bytes = UnHex(c.tok, 3);
          ports_[p].rx.insert(ports_[p].rx.end(), bytes.begin(), bytes.end());
          Event("uart", "\"op\":\"rx\",\"port\":\"" + p + "\",\"hex\":\"" + Hex(bytes) + "\"");
        } else if (what == "rx-file" && !c.applied && c.tok.size() >= 4) {
          c.applied = true;
          std::ifstream f(c.tok[3], std::ios::binary);
          const std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
          ports_[p].rx.insert(ports_[p].rx.end(), bytes.begin(), bytes.end());
          Event("uart", "\"op\":\"rx-file\",\"port\":\"" + p + "\",\"bytes\":" + std::to_string(bytes.size()));
        } else if (p == port && what == "timeout") {
          *timeout = true;
        } else if (p == port && what == "fault" && c.tok.size() >= 4) {
          const std::string& k = c.tok[3];
          const bool match = (k == "open" && op == Op::kOpen) || (k == "read" && op == Op::kRead) ||
                             (k == "write" && op == Op::kWrite);
          if (!*fault && match && Consume(&c)) {
            *fault = true;
            Event("uart", "\"op\":\"fault\",\"port\":\"" + p + "\",\"kind\":\"" + k + "\"");
          }
        }
      } catch (...) {
        // nieczytelna linia ctl - pomijamy
      }
    });
  }

  std::mutex m_;
  std::map<std::string, Port> ports_;
  std::map<int, std::string> handles_;
  int next_handle_{1000};
};

}  // namespace sim
}  // namespace core
}  // namespace srp

#endif  // CORE_SIM_SIM_UART_HPP_
