/**
 * @file sim_ctl.hpp
 * @copyright Copyright (c) 2025
 * @brief Wspolny kanal sterowania driverow sim (gpio/i2c/temp) z zewnatrz.
 *
 * Katalog: $SRP_SIM_DIR (domyslnie /tmp/srpsim).
 *   ctl            - polecenia (1 na linie), czytane ponownie przy zmianie pliku
 *   events.jsonl   - dziennik zdarzen (JSON lines, dopisywany)
 *   state/...      - aktualny stan (gpio/<pin>, i2c/<addr>.hex)
 * Skladnia polecen i opis: docs/audit/sim_drivers.md
 * Deterministycznie: brak losowosci; czas tylko z "after <ms>" liczony od
 * momentu pierwszego zobaczenia linii przez dany proces.
 */
#ifndef CORE_SIM_SIM_CTL_HPP_
#define CORE_SIM_SIM_CTL_HPP_

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>  // NOLINT
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <mutex>  // NOLINT
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace srp {
namespace core {
namespace sim {

inline std::string Dir() {
  const char* e = std::getenv("SRP_SIM_DIR");
  return (e != nullptr && *e != 0) ? std::string(e) : std::string("/tmp/srpsim");
}

inline int64_t SteadyMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count();
}

inline int64_t WallMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
}

inline void MkDirs(const std::string& path) {
  std::string cur;
  for (size_t i = 0; i <= path.size(); ++i) {
    if (i == path.size() || path[i] == '/') {
      if (!cur.empty()) {
        ::mkdir(cur.c_str(), 0777);
      }
    }
    if (i < path.size()) {
      cur += path[i];
    }
  }
}

/// Dopisuje linie JSON do events.jsonl (O_APPEND, atomowo miedzy procesami).
inline void Event(const std::string& src, const std::string& fields) {
  MkDirs(Dir());
  std::string line = "{\"t\":" + std::to_string(WallMs()) + ",\"src\":\"" + src + "\"" +
                     (fields.empty() ? "" : "," + fields) + "}\n";
  int fd = ::open((Dir() + "/events.jsonl").c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
  if (fd >= 0) {
    ssize_t r = ::write(fd, line.data(), line.size());
    (void)r;
    ::close(fd);
  }
}

/// Zapis stanu (tmp + rename), rel np. "gpio/13".
inline void WriteState(const std::string& rel, const std::string& content) {
  const std::string full = Dir() + "/state/" + rel;
  MkDirs(full.substr(0, full.rfind('/')));
  const std::string tmp = full + ".tmp";
  {
    std::ofstream f(tmp, std::ios::trunc);
    f << content;
  }
  std::rename(tmp.c_str(), full.c_str());
}

inline std::string Hex(const std::vector<uint8_t>& v) {
  static const char* d = "0123456789abcdef";
  std::string s;
  for (auto b : v) {
    s += d[b >> 4];
    s += d[b & 15];
  }
  return s;
}

/// Hex ("0a0b", "0x0a 0b") -> bajty.
inline std::vector<uint8_t> UnHex(const std::vector<std::string>& toks, size_t from) {
  std::string s;
  for (size_t i = from; i < toks.size(); ++i) {
    std::string t = toks[i];
    if (t.rfind("0x", 0) == 0 || t.rfind("0X", 0) == 0) {
      t = t.substr(2);
    }
    s += t;
  }
  std::vector<uint8_t> out;
  for (size_t i = 0; i + 1 < s.size(); i += 2) {
    out.push_back(static_cast<uint8_t>(std::stoi(s.substr(i, 2), nullptr, 16)));
  }
  return out;
}

struct Cmd {
  std::vector<std::string> tok;
  std::string text;
  int64_t after_ms{0};
  int64_t seen_ms{0};
  bool applied{false};
  int64_t left{-1};  // "count N": ile razy polecenie-usterka moze zadzialac
};

/// Wartosc po kluczu, np. Opt(c, "count").
inline std::optional<std::string> Opt(const Cmd& c, const std::string& key) {
  for (size_t i = 0; i + 1 < c.tok.size(); ++i) {
    if (c.tok[i] == key) {
      return c.tok[i + 1];
    }
  }
  return std::nullopt;
}

/// true gdy usterka ma zadzialac (zuzywa "count").
inline bool Consume(Cmd* c) {
  if (c->left < 0) {
    return true;
  }
  if (c->left == 0) {
    return false;
  }
  --c->left;
  return true;
}

class Ctl {
 public:
  static Ctl& Get() {
    static Ctl c;
    return c;
  }

  /// Odswieza plik ctl i wola fn dla polecen, ktorych opoznienie "after" minelo.
  void Visit(const std::function<void(Cmd&)>& fn) {
    std::lock_guard<std::mutex> l(m_);
    Refresh();
    const int64_t now = SteadyMs();
    for (auto& c : cmds_) {
      if (now - c.seen_ms >= c.after_ms) {
        fn(c);
      }
    }
  }

 private:
  void Refresh() {
    struct stat st;
    const std::string path = Dir() + "/ctl";
    if (::stat(path.c_str(), &st) != 0) {
      if (have_) {
        cmds_.clear();
        have_ = false;
      }
      return;
    }
    if (have_ && st.st_mtim.tv_sec == sec_ && st.st_mtim.tv_nsec == nsec_ && st.st_size == size_) {
      return;
    }
    have_ = true;
    sec_ = st.st_mtim.tv_sec;
    nsec_ = st.st_mtim.tv_nsec;
    size_ = st.st_size;
    std::ifstream f(path);
    std::string line;
    std::vector<Cmd> fresh;
    const int64_t now = SteadyMs();
    while (std::getline(f, line)) {
      std::istringstream is(line);
      std::vector<std::string> t;
      std::string w;
      while (is >> w) {
        t.push_back(w);
      }
      if (t.empty() || t[0][0] == '#') {
        continue;
      }
      Cmd c;
      c.text = line;
      try {
        if (t[0] == "after" && t.size() >= 3) {
          c.after_ms = std::stoll(t[1]);
          t.erase(t.begin(), t.begin() + 2);
        }
        c.tok = t;
        auto cnt = Opt(c, "count");
        if (cnt) {
          c.left = std::stoll(*cnt);
        }
      } catch (...) {
        continue;
      }
      const size_t i = fresh.size();
      if (i < cmds_.size() && cmds_[i].text == c.text) {
        fresh.push_back(cmds_[i]);  // bez zmian - zachowaj stan (applied/left/seen)
      } else {
        c.seen_ms = now;
        fresh.push_back(c);
      }
    }
    cmds_ = fresh;
  }

  std::mutex m_;
  std::vector<Cmd> cmds_;
  bool have_{false};
  time_t sec_{0};
  long nsec_{0};  // NOLINT
  off_t size_{0};
};

}  // namespace sim
}  // namespace core
}  // namespace srp

#endif  // CORE_SIM_SIM_CTL_HPP_
