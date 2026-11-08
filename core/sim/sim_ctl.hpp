/**
 * @file sim_ctl.hpp
 * @copyright Copyright (c) 2025
 * @brief Wspolny kanal sterowania driverow sim (gpio/i2c/temp) z zewnatrz.
 *
 * Katalog: $SRP_SIM_DIR (domyslnie /tmp/srpsim).
 *   ctl            - polecenia (1 na linie), czytane ponownie przy zmianie pliku
 *   events.jsonl   - dziennik zdarzen (JSON lines, dopisywany)
 *   state/...      - aktualny stan (gpio/<pin>, i2c/<addr>.hex)
 * Skladnia polecen i opis: core/sim/README.md
 * Deterministycznie: brak losowosci; czas tylko z "after <ms>" liczony od
 * momentu pierwszego zobaczenia linii przez dany proces.
 */
#ifndef CORE_SIM_SIM_CTL_HPP_
#define CORE_SIM_SIM_CTL_HPP_

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cctype>
#include <chrono>  // NOLINT
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <functional>
#include <mutex>  // NOLINT
#include <optional>
#include <sstream>
#include <stdexcept>
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

inline void MkDirsAlways(const std::string& path) {
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

/// mkdir -p, ale bez ponawiania dla tej samej sciezki w tym watku (Event/WriteState wolane bardzo czesto).
inline void MkDirs(const std::string& path) {
  thread_local std::string last;
  if (last != path) {
    MkDirsAlways(path);
    last = path;
  }
}

/// Dopisuje linie JSON do events.jsonl (O_APPEND, atomowo miedzy procesami).
inline void Event(const std::string& src, const std::string& fields) {
  MkDirs(Dir());
  std::string line = "{\"t\":" + std::to_string(WallMs()) + ",\"src\":\"" + src + "\"" +
                     (fields.empty() ? "" : "," + fields) + "}\n";
  int fd = ::open((Dir() + "/events.jsonl").c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
  if (fd < 0) {  // the directory may have been removed from outside (rm -rf $SRP_SIM_DIR): recreate and retry
    MkDirsAlways(Dir());
    fd = ::open((Dir() + "/events.jsonl").c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
  }
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
  const std::string tmp = full + "." + std::to_string(::getpid()) + ".tmp";
  std::ofstream f(tmp, std::ios::trunc);
  if (!f.is_open()) {  // directory removed from outside: recreate and retry
    MkDirsAlways(full.substr(0, full.rfind('/')));
    f.open(tmp, std::ios::trunc);
  }
  f << content;
  f.close();
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

/// Calosc tokenu jako liczba (base 10 albo 0 = auto); smieci na koncu ("12abc") -> std::invalid_argument.
inline int64_t ParseNum(const std::string& s, int base = 10) {
  size_t pos = 0;
  const int64_t v = std::stoll(s, &pos, base);
  if (pos != s.size()) {
    throw std::invalid_argument("trailing characters in number: " + s);
  }
  return v;
}

/// Hex ("0a0b", "0x0a 0b") -> bajty. Nieparzysta liczba cyfr albo znak spoza hex -> std::invalid_argument
/// (linia ctl jest wtedy pomijana, nic nie jest cicho obcinane).
inline std::vector<uint8_t> UnHex(const std::vector<std::string>& toks, size_t from) {
  std::string s;
  for (size_t i = from; i < toks.size(); ++i) {
    std::string t = toks[i];
    if (t.rfind("0x", 0) == 0 || t.rfind("0X", 0) == 0) {
      t = t.substr(2);
    }
    s += t;
  }
  if (s.size() % 2 != 0) {
    throw std::invalid_argument("odd number of hex digits");
  }
  std::vector<uint8_t> out;
  for (size_t i = 0; i < s.size(); i += 2) {
    if (!std::isxdigit(static_cast<unsigned char>(s[i])) || !std::isxdigit(static_cast<unsigned char>(s[i + 1]))) {
      throw std::invalid_argument("non-hex character");
    }
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
    const int64_t now = SteadyMs();
    if (::stat(path.c_str(), &st) != 0) {
      if (have_) {
        cmds_.clear();
        have_ = false;
      }
      key_valid_ = false;
      empty_since_ = 0;
      return;
    }
    if (key_valid_ && st.st_mtim.tv_sec == sec_ && st.st_mtim.tv_nsec == nsec_ && st.st_size == size_ &&
        st.st_ino == ino_) {
      return;
    }
    // `cat > ctl` (truncate + write) shows an empty file for a moment: keep the state for 1 s, otherwise every
    // line would be seen as new afterwards (replayed rx / value).
    if (st.st_size == 0 && have_ && !cmds_.empty()) {
      if (empty_since_ == 0) {
        empty_since_ = now;
      }
      if (now - empty_since_ < 1000) {
        return;
      }
    } else {
      empty_since_ = 0;
    }
    std::ifstream f(path, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    // the last line without '\n' may be half-written: ignore it and read the file again on the next Visit
    const bool partial = !content.empty() && content.back() != '\n';
    if (partial) {
      const size_t nl = content.rfind('\n');
      content.erase(nl == std::string::npos ? 0 : nl + 1);
    }
    std::istringstream in(content);
    std::string line;
    std::vector<Cmd> fresh;
    std::vector<bool> used(cmds_.size(), false);
    while (std::getline(in, line)) {
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
      // match by content (first unused old command with the same text), not by position: inserting or removing a
      // line above must not reset applied/left/seen of the following ones
      bool matched = false;
      for (size_t j = 0; j < cmds_.size(); ++j) {
        if (!used[j] && cmds_[j].text == c.text) {
          used[j] = true;
          fresh.push_back(cmds_[j]);
          matched = true;
          break;
        }
      }
      if (!matched) {
        c.seen_ms = now;
        fresh.push_back(c);
      }
    }
    cmds_ = fresh;
    have_ = true;
    key_valid_ = !partial;
    sec_ = st.st_mtim.tv_sec;
    nsec_ = st.st_mtim.tv_nsec;
    size_ = st.st_size;
    ino_ = st.st_ino;
  }

  std::mutex m_;
  std::vector<Cmd> cmds_;
  bool have_{false};
  bool key_valid_{false};
  int64_t empty_since_{0};
  time_t sec_{0};
  long nsec_{0};  // NOLINT
  off_t size_{0};
  ino_t ino_{0};
};

}  // namespace sim
}  // namespace core
}  // namespace srp

#endif  // CORE_SIM_SIM_CTL_HPP_
