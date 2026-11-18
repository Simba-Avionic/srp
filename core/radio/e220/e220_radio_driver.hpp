/**
 * Copyright (c) 2024
 *
 * @file e220_radio_driver.hpp
 * @brief Driver modemu Ebyte E220-400T30S jako ara::com::mavlink::IRadioDriver (plugin `ebyte_e220`
 * dla MavlinkDaemon, spec rev. 10).
 *
 * Start(): tryb konfiguracji (M0=M1=1, UART 9600 8N1) -> odczyt rejestrow 00H-05H (C1) ->
 * zapis (C0) wylacznie rejestrow roznych od docelowych -> odczyt weryfikujacy -> tryb
 * normalny (M0=M1=0) -> UART z predkoscia z REG0 -> watki drivera. 3 proby, potem blad kRetryable.
 *
 * Model zdarzeniowy (platforma: radio_driver.h):
 *  - watek RX czeka na dane z UART (poll) i woła OnReceive z kawalkami strumienia,
 *  - Send() jest NIEBLOKUJACE: ramka (<= 400 B) jest przyjmowana w calosci, jesli mieści sie w
 *    szacowanym buforze modulu (model pakiet po pakiecie, patrz E220Timing::air_drain_factor);
 *    inaczej kBusy, a watek TX woła OnTxReady(), gdy miejsce sie zwolni,
 *  - watek TX zapisuje przyjete ramki do UART dopiero przy AUX wysokim (modul wolny),
 *  - utrata UART/modulu (blad odczytu/zapisu, AUX nie wraca) = OnError(kPersistent, fatal).
 * Po Stop() zero callbackow (watki zjoinowane). Platforma nie zna modemu: tu jest cala wiedza o
 * rejestrach i pinach.
 */
#ifndef CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_
#define CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_

#include <atomic>
#include <chrono>  // NOLINT
#include <condition_variable>  // NOLINT
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <span>  // NOLINT
#include <stop_token>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "ara/com/mavlink/radio_driver.h"
#include "core/radio/e220/e220_registers.hpp"
#include "core/radio/e220/i_e220_pins.hpp"
#include "core/uart/Iuart_driver.hpp"

namespace srp {
namespace core {
namespace radio {
namespace e220 {

/// Czasy (domyslne wg instrukcji; testy je skracaja).
struct E220Timing {
  std::chrono::milliseconds aux_timeout{2000};       ///< czekanie na AUX wysoki
  std::chrono::milliseconds aux_poll{1};
  /// max czekanie na opadniecie AUX po zmianie trybu (5.2.4: ~10 ms)
  std::chrono::milliseconds mode_switch_wait{30};
  std::chrono::milliseconds mode_settle{5};          ///< pauza po AUX wysokim
  std::chrono::milliseconds command_timeout{500};    ///< odpowiedz na komende HEX
  /// po zapisie ramki czekamy tyle, az AUX opadnie (modul zglasza odbior bajtow, T1 ~1 ms, 5.2.3),
  /// zanim kolejny zapis uzna AUX=1 za "bufor pusty"
  std::chrono::milliseconds aux_fall_wait{15};
  std::chrono::milliseconds retry_delay{100};
  std::chrono::milliseconds read_slice{50};          ///< kwant czekania RX (Stop() wybudza w tym czasie)
  /// Pacing bufora 400 B modulu: AUX nie mowi, ile bajtow siedzi jeszcze w buforze (przy pakiecie
  /// krotszym niz dlugosc pakietu AUX jest wysoki mimo danych w locie, 5.2.3/7.1.7), wiec driver
  /// modeluje bufor pakiet po pakiecie: modul tnie dane na pakiety <= dlugosc pakietu (pelny pakiet
  /// od razu, niepelny po przerwie ramki UART), kazdy pakiet zajmuje miejsce DO KONCA swojego czasu
  /// antenowego (bajty*8 / (szybkosc radiowa * air_drain_factor)) i dopiero wtedy je zwalnia.
  /// Wspolczynnik < 1 = margines (przepustowosc LoRa jest nizsza od bps/8; kalibracja na HW).
  double air_drain_factor{0.8};
  /// Zalozone opoznienie, po ktorym modul uznaje ramke UART za zakonczona (niepelny pakiet startuje);
  /// instrukcja: ~3 bajty ciszy (5.2.3). Dluzsze od rzeczywistego = ostrozniej (miejsce zwalnia pozniej).
  std::chrono::milliseconds module_frame_gap{5};
  /// TYLKO TESTY (kontrola negatywna E2E): Send nie liczy modelu bufora, a zapis nie czeka na AUX.
  bool test_ignore_aux_on_write{false};
  int attempts{3};
};

class E220RadioDriver final : public ara::com::mavlink::IRadioDriver {
 public:
  /// Maksymalny jednorazowy zapis do bufora UART modulu (5.2.3, str. 25) = najwieksza ramka.
  static constexpr std::size_t kModuleBuffer = 400;

  E220RadioDriver(std::string name, std::string uart_port, E220Config config,
                  std::shared_ptr<uart::IUartDriver> uart, std::shared_ptr<IE220Pins> pins,
                  E220Timing timing = {});
  /// Driver z bledem konfiguracji: Start() zwraca kPersistent z `config_error` (czytelny log daemona).
  static std::shared_ptr<E220RadioDriver> WithConfigError(std::string name, std::string config_error);
  ~E220RadioDriver() override;

  ara::core::Result<void> Start(ara::com::mavlink::IRadioListener& listener,  // NOLINT(runtime/references)
                                std::stop_token token) noexcept override;
  void Stop() noexcept override;
  ara::core::Result<void> Send(std::span<const std::uint8_t> frame) noexcept override;
  std::string Name() const noexcept override { return "e220:" + name_; }

  /// Ostatnio odczytane rejestry (diagnostyka/testy).
  Registers last_registers() const;
  /// TYLKO TESTY/narzedzia: czeka, az wszystkie przyjete ramki zostana zapisane do UART (kolejka pusta).
  bool WaitTxQueueEmpty(std::chrono::milliseconds timeout);

 private:
  using Clock = std::chrono::steady_clock;

  /// Jedna proba konfiguracji; nullopt = sukces, inaczej opis bledu.
  std::optional<std::string> ConfigureOnce(const std::stop_token& token);
  bool WaitAux(bool level, std::chrono::milliseconds timeout, const std::stop_token& token);
  bool SwitchMode(bool config_mode, const std::stop_token& token, std::string* error);
  bool OpenUart(std::uint32_t baud, uart::Parity parity);
  void FlushRx();
  /// Wysyla komende i czeka na `expected` bajtow odpowiedzi.
  std::optional<std::vector<std::uint8_t>> Exchange(const std::vector<std::uint8_t>& cmd,
                                                    std::size_t expected, const std::stop_token& token,
                                                    std::string* error);
  bool ReadRegisters(Registers* regs, const std::stop_token& token, std::string* error);
  bool SleepFor(std::chrono::milliseconds d, const std::stop_token& token);

  void RxLoop(std::stop_token token);
  void TxLoop(std::stop_token token);
  /// Zapisuje jedna przyjeta ramke do UART (AUX wysoki przed i opadniecie po); false = blad fatalny.
  bool WriteFrame(const std::vector<std::uint8_t>& frame, const std::stop_token& token);
  void Fatal(ara::core::ErrorCode e);
  void StopThreadsLocked();

  // model bufora modulu (pod mutex_), patrz E220Timing::air_drain_factor
  void ModelAdvance(Clock::time_point now);
  std::size_t ModelFree() const {
    const std::size_t used = model_pending_ + model_inflight_ + queued_bytes_;
    return used >= kModuleBuffer ? 0 : kModuleBuffer - used;
  }
  Clock::time_point ModelNextEvent() const;

  const std::string name_;
  const std::string uart_port_;
  const E220Config config_;
  const E220Timing timing_;
  std::string config_error_;
  std::shared_ptr<uart::IUartDriver> uart_;
  std::shared_ptr<IE220Pins> pins_;

  std::mutex lifecycle_mutex_;  // Start/Stop
  std::mutex stop_mutex_;  // chroni stop_ (Start podmienia, Stop/Fatal zadaja stopu)
  std::stop_source stop_;
  std::thread rx_thread_;
  std::thread tx_thread_;
  ara::com::mavlink::IRadioListener* listener_{nullptr};

  std::mutex mutex_;  // stan TX (kolejka, model, running_)
  std::condition_variable cv_;
  bool running_{false};
  std::atomic<bool> fatal_{false};
  std::deque<std::vector<std::uint8_t>> queue_;
  std::size_t queued_bytes_{0};  // przyjete, jeszcze nie zapisane do UART
  bool tx_ready_owed_{false};
  std::size_t busy_need_{0};  // rozmiar ramki odrzuconej kBusy: OnTxReady, gdy tyle miejsca
  std::size_t model_pending_{0};            // bajty jeszcze nie w pakiecie
  std::size_t model_inflight_{0};           // bajty pakietu w powietrzu (zajmuja bufor do konca)
  Clock::time_point model_inflight_end_{};  // koniec czasu antenowego pakietu / ostatniego pakietu
  Clock::time_point model_last_write_{};

  mutable std::mutex regs_mutex_;
  Registers last_registers_{};
};

}  // namespace e220
}  // namespace radio
}  // namespace core
}  // namespace srp

#endif  // CORE_RADIO_E220_E220_RADIO_DRIVER_HPP_
