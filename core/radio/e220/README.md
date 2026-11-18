# Driver modemu Ebyte E220-400T30S (`core/radio/e220`)

Driver modemu LoRa **E220-400T30S** jako **plugin `ebyte_e220`** dla MavlinkDaemona (`ara::com::mavlink::IRadioDriver`
z `srp_platform`, spec rev. 10). Platforma o modemie nic nie wie — cała wiedza o rejestrach, pinach M0/M1/AUX i buforze modułu jest tutaj.
Daemon ładuje `libebyte_e220.so` z katalogu driverów (`driver_dir`), a łącze (`links.lora.driver = "ebyte_e220"`) wskazuje plik configu drivera
(`driver_config`) — patrz „Konfiguracja” i „Deployment EC”.

## Źródło wiedzy o module

Wszystkie wartości rejestrów, tryby, czasy i limity pochodzą z oficjalnej instrukcji:
**Ebyte „E220-xxxTxxx Series User Manual”** (nagłówek stron: „E220-T Series User Manual”), **rev. 1.8 z 2026-11-12, 54 strony**
(kopia robocza: `sha256 e8028fea…b750fd`). Instrukcja nie numeruje tabel, więc cytujemy sekcje i strony:

| Sekcja (str.) | Czego dotyczy w drivera |
|---|---|
| 5.1 (17) | tryby z pinów M1/M0: `00` transmisja, `11` konfiguracja |
| 5.2.1 (24), 5.2.4 (27–28) | AUX: po zmianie trybu ~10 ms nisko, zmianę trybu robimy przy AUX=1 i czekamy na powrót AUX=1 |
| 5.2.3 (25–26) | wewnętrzny bufor UART modułu **400 B**; nadmiar jest tracony → `Write` dzieli dane na porcje ≤ 400 B i każdą wysyła dopiero przy AUX=1 |
| 6.1 (30–31) | komendy trybu konfiguracji **tylko przy 9600 8N1**: `C0` zapis trwały, `C1` odczyt, odpowiedź `C1 start len params`, błąd formatu `FF FF FF` |
| 6.2–6.3 (31–34) | rejestry 00H–07H: ADDH/ADDL, REG0 (baud UART [7:5], parzystość [4:3], szybkość radiowa [2:0]), REG1 (długość pakietu [7:6], RSSI otoczenia [5], soft-switch [2], moc [1:0]), REG2 (kanał), REG3 (bajt RSSI [7], transmisja fixed [6], LBT [4], WOR [2:0]); CRYPT 06H/07H tylko zapis |
| 6.4 (35) | ustawienia fabryczne `00 00 62 00 17 03 00 00` |

Znane błędy/luki dokumentu (driver postępuje konserwatywnie, szczegóły: `sim/README.md`):
6.4 podaje długość `09`, a wymienia 8 bajtów parametrów; w REG0 wartości `000/001/010` szybkości radiowej oznaczają tę samą 2,4 kbps
(driver zapisuje `010` i traktuje wszystkie trzy jako zgodne); instrukcja nie podaje czasu resetu po wyjściu z trybu konfiguracji.
**Driver nie był jeszcze sprawdzony na prawdziwym module** — testy używają atrapy `e220_sim.py` opartej na tej samej instrukcji.

## Co robi `Open()`

1. Otwiera UART portu modemu **9600 8N1** (wymóg trybu konfiguracji), czeka na AUX=1, ustawia **M0=1, M1=1** i czeka na powrót AUX=1.
2. `C1 00 06` — odczyt rejestrów 00H–05H.
3. Liczy docelowe rejestry z konfiguracji i porównuje. `C0` wysyłane jest **tylko dla rejestrów, które się różnią** — jeden `C0 <reg> 01 <wartość>`
   na rejestr (EEPROM modułu ma ograniczoną liczbę zapisów). Drugi start bez zmian konfiguracji = **0 bajtów `C0`**.
4. Jeśli cokolwiek zapisano: ponowny odczyt `C1` i weryfikacja.
5. **M0=0, M1=0**, AUX=1, i — gdy baud/parzystość z REG0 różnią się od 9600 8N1 — ponowne otwarcie UART z parametrami z konfiguracji.

Do 3 prób (przerwa 100 ms); po trzeciej porażce `Start()` zwraca `kRetryable` z opisem przyczyny (AUX, brak odpowiedzi, `FF FF FF`, weryfikacja…).
Niepoprawna konfiguracja → `kPersistent` bez dotykania sprzętu. `stop_token`/`Stop()` przerywają `Start()` w trakcie czekania na AUX/odpowiedź.

**Narzucone przez driver** (niezależnie od konfiguracji; reszta bitów zarezerwowanych/WOR zostaje taka, jaka jest w module):

- REG3: **transmisja transparentna** (bit6 = 0 — fixed-point „zjadałby” 3 bajty każdej ramki) i **bez bajtu RSSI** (bit7 = 0 — dopisywałby bajt do każdej odebranej porcji);
- REG1: wyłączony RSSI otoczenia (bit5 = 0) i soft-switch (bit2 = 0 — tryb ustawiają piny);
- CRYPT 06H/07H nie są używane (szyfrowanie radiowe poza zakresem; MAVLink ma własny podpis).

## Model zdarzeniowy: `Start()` / `Send()` / callbacki

Driver ma własne wątki (po udanym `Start()`); daemon nie woła blokującego `Read`:

- **RX:** wątek czeka na dane z UART (`poll()`) i woła `OnReceive(kawałek)` — dowolny kawałek strumienia (też połowa ramki); błąd odczytu UART
  (utrata urządzenia) → `OnError(kPersistent, fatal)`.
- **`Send(ramka)` — NIEBLOKUJĄCE:** ramka (≤ 400 B) jest przyjmowana w całości albo wcale. Przyjęta trafia do kolejki, a wątek TX zapisuje ją do UART
  dopiero przy **AUX=1** (moduł nie zgubi bajtów, `bytes_while_busy`/`tx_overflow` atrapy = 0) i czeka do `aux_fall_wait` (15 ms), aż AUX opadnie
  (moduł przyjął bajty, T1 ≈ 1 ms, 5.2.3) — bez tego kolejny zapis widziałby „stare” AUX=1 i mógł przepełnić bufor 400 B.
  Brak miejsca w buforze modułu → **`kBusy`** (nic nie przyjęto); gdy miejsce się zwolni, driver woła **`OnTxReady()`** (raz na `kBusy`, także gdy
  miejsce zwolniło się tuż po zwróceniu `kBusy`). Ramka > 400 B → `kInvalidArgument`. AUX, który nie wraca na wysoki w `aux_timeout` (2 s), albo błąd
  zapisu UART = moduł nieużywalny → `OnError(kPersistent, fatal)`; daemon robi `Stop()` i po `restart_delay_ms` `Start()`.
- **Model bufora 400 B (pacing):** AUX nie mówi, ile bajtów siedzi jeszcze w buforze (przy danych krótszych niż długość pakietu AUX jest wysoki
  mimo bajtów „w locie”, 5.2.3/7.1.7), więc driver modeluje bufor **pakiet po pakiecie** (nie ciągłym drenażem): moduł tnie dane na pakiety
  ≤ `packet_length` (pełny pakiet od razu, niepełny po przerwie ramki UART), a pakiet zajmuje miejsce **do końca swojego czasu antenowego**
  (bajty × 8 / (`air_rate` × `air_drain_factor`), 0,8) i dopiero wtedy je zwalnia — tak jak atrapa i instrukcja. `Send` przyjmuje ramkę, gdy mieści się
  w wolnym miejscu (przyjęte-niezapisane + pakiet w locie + niepełny pakiet ≤ 400 B). Ramki o zmiennej długości (np. 200 × 12…279 B) przechodzą bez
  utraty; kosztem jest przepustowość ≤ 80% szybkości radiowej. Rzeczywista przepustowość LoRa jest niższa niż `air_rate`/8, więc `air_drain_factor`
  trzeba **skalibrować na prawdziwym module** (plan HW: seria ramek, bajty nadane = odebrane).
- **`Stop()`:** zatrzymuje oba wątki; po powrocie **zero callbacków** (także gdy daemon czekał na `OnTxReady` po `kBusy`); przerywa też trwające `Start()`.
  `Start()` po `Stop()` działa (modem konfiguruje się od nowa; bez zmian w configu = 0 zapisów `C0`).
- Zgodność z kontraktem `IRadioDriver` sprawdza wspólny zestaw testów platformy (`radio_driver_contract_test`, `ut/test_e220_contract.cpp`).

**Plugin nie używa `ara::log`, srp `UartDriver` ani `IGpioDriver`:** biblioteka `.so` ma własne kopie bibliotek statycznych, więc singleton loggera hosta
jest w niej niezainicjowany, a konstruktory tych klas go wołają (plugin by padł). Sprzęt obsługują `termios_uart` (termios + `poll`, bez `TIOCEXCL`)
i `sysfs_e220_pins` (sysfs GPIO); błędy wracają przez `Start()`/`OnError` do logu daemona.

## Konfiguracja (plik drivera, `driver_config` łącza)

Własny plik drivera (format zna tylko driver); ścieżkę podaje config daemona. Plik jest wczytywany przy **każdym** `Start()`, więc poprawiony plik
działa po ponownym starcie łącza (daemon ponawia `Start()` po `restart_delay_ms`, domyślnie 30 s — bez restartu procesu). Błąd pliku = `Start()` z `kPersistent`
i czytelnym opisem w logu daemona (nazwa pliku, pole, powód), np. `plik '/srp/opt/mavlink_daemon/ebyte_e220_lora.json': nieznane pole 'bogus'`.

```json
{
  "uart": "/dev/ttyS1",
  "pins": { "m0": 17, "m1": 27, "aux": 22 },
  "e220": { "address": 4660, "channel": 18, "air_rate": 9600, "power_dbm": 27, "uart_baudrate": 57600 }
}
```

| Pole | Znaczenie |
|---|---|
| `uart` | urządzenie UART modułu (wymagane) |
| `pins.m0`, `pins.m1`, `pins.aux` | numery sysfs GPIO (`/sys/class/gpio`; wymagane, różne); `pins.ctl_dir` (piny w plikach atrapy) tylko w pluginie testowym |
| `e220` | parametry radia poniżej (opcjonalne; brak pola = wartość fabryczna); nieznany klucz = błąd |

Parametry radia (`e220`); walidacja zakresów dla **E220-400T30S**:

| Pole | Wartości | Rejestr |
|---|---|---|
| `address` | 0–65535 | ADDH/ADDL |
| `channel` | 0–83 (częstotliwość 410,125 + CH MHz) | REG2 |
| `air_rate` | 2400, 4800, 9600, 19200, 38400, 62500 (bps) | REG0 [2:0] |
| `power_dbm` | 30, 27, 24, 21 | REG1 [1:0] |
| `uart_baudrate` | 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 | REG0 [7:5] |
| `parity` | `"8N1"`, `"8O1"`, `"8E1"` | REG0 [4:3] |
| `packet_length` | 200, 128, 64, 32 (B) | REG1 [7:6] |
| `lbt` | `true`/`false` (listen before talk) | REG3 bit4 |
| `wor_cycle` | 0–7 (opcjonalne; brak = WOR bez zmian) | REG3 [2:0] |

Obie strony łącza (EC/FC i GS) muszą mieć ten sam `channel`, `air_rate` i zgodne `address`; `parity` i `uart_baudrate` dotyczą tylko UART
między hostem a swoim modułem, ale przy `8O1`/`8E1` ten sam tryb musi mieć druga strona kabla UART (np. USB-UART w GS — narzędzie GS
konfiguruje moduł, a potem host musi otwierać port z tą samą parzystością).

## Deployment EC

`deployment/platform/mavlink_daemon` (analogicznie do `someip_demon`) wkłada do pakietu EC: config daemona
(`/srp/opt/mavlink_daemon/mavlink_daemon_config.json`: `system_id`/`component_id`, `driver_dir`, łącze `lora` → `ebyte_e220`), plik drivera
(`ebyte_e220_lora.json`) i plugin `libebyte_e220.so` (`drivers/`). Plugin buduje reguła platformy `mavlink_driver_plugin` (cel `//core/radio/e220:ebyte_e220`).
Numery GPIO (M0/M1/AUX) i UART (`/dev/ttyS1`) to dane płytki — wartości w repo są przykładowe, **do potwierdzenia na sprzęcie EC**.
Aplikacje z `on: mavlink` są klientami daemona (nie tworzą drivera ani łącza).

Piny AUX odczytuje się odpytywaniem co 1 ms (sysfs bez przerwań). `SysfsE220Pins::ReadAux` zwraca błąd (a driver czeka do timeoutu), gdy
wartość nie jest 0/1 — awaria pinu nie jest traktowana jak „moduł wolny”. Czasy (timeout AUX 2 s, czas przełączania trybu 30 ms,
timeout komendy 500 ms) są polami `E220Timing`.

## Narzędzie dla stacji naziemnej (GS)

`tools/radio/e220_config.py` — konfiguracja modemu GS przez USB-UART (bez zależności poza biblioteką standardową Pythona),
ten sam JSON i te same reguły co w driverze:

```
bazel run //tools/radio:e220_config -- --port /dev/ttyUSB0 --config gs_e220.json --check   # 0 = zgodne, 1 = różne, 2 = błąd
bazel run //tools/radio:e220_config -- --port /dev/ttyUSB0 --config gs_e220.json --apply   # zapis tylko różniących się rejestrów + weryfikacja
```

**Tryb konfiguracji (M0=1, M1=1) ustawiasz ręcznie** zworkami/przełącznikami modułu przed uruchomieniem narzędzia i po konfiguracji
wracasz do M0=0, M1=0 (USB-UART nie steruje tymi pinami). Konfiguracja działa tylko przy 9600 8N1.

## Testy

| Co | Gdzie | Uruchomienie |
|---|---|---|
| rejestry, JSON, walidacja | `ut/test_e220_registers.cpp` | `bazel test //core/radio/e220:test_e220_registers` |
| `Start()` (idempotencja, AUX, retry, baud), `Send`/`kBusy`/`OnTxReady`, błędy fatalne, `Stop` (podstawka modułu) | `ut/test_e220_driver.cpp` | `…:test_e220_driver` |
| kontrakt `IRadioDriver` z platformy (w tym Stop w trakcie oczekiwania na `OnTxReady`, zero callbacków po Stop) | `ut/test_e220_contract.cpp` | `…:test_e220_contract` |
| plik configu drivera, fabryka (błąd configu = `Start()` z opisem, ponowne wczytanie przy `Start()`) | `ut/test_e220_driver_config.cpp` | `…:test_e220_driver_config` |
| UART termios (pty) i piny sysfs (katalog tymczasowy) | `ut/test_e220_io.cpp` | `…:test_e220_io` |
| E2E z atrapą modemu `e220_sim.py` bez daemona: 1. start zapisuje różnice, 2. start 0 zapisów, serie ramek 20 × 200 B / 200 × 12…279 B / serie testera: komplet dostarczony, `tx_overflow` = 0, kontrola negatywna | `ut/e220_driver_series_test.py` (`e220_driver_probe`) | `…:e220_driver_series_test` |
| E2E: MavlinkDaemon z platformy ładuje plugin `.so`, łącze `lora` na atrapie, 2 klientów IPC: event + metoda ↔ pymavlink, 2. start 0 zapisów `C0`, zły plik configu → błąd w logu i start po poprawce | `ut/e220_plugin_e2e_test.py` | `…:e220_plugin_e2e_test` |
| narzędzie GS na atrapie | `tools/radio/e220_config_test.py` | `bazel test //tools/radio:e220_config_test` |

Testy E2E i narzędzie GS używają atrapy z tego repo (`//core/radio/e220/sim:e220_sim_lib`, opis: `sim/README.md`); sam driver wymaga platformy z MavlinkDaemonem
(gałąź `mav/d2-client`/`mavlink-daemon`); `FakeE220Module` (`testing/`) to uproszczona podstawka do testów jednostkowych drivera.
