# Atrapa modemu Ebyte E220-400T30S (`e220_sim.py`)

Atrapa do testów sterownika modemu (`E220RadioDriver`) i E2E. Pisana **niezależnie od drivera**,
wyłącznie z dokumentacji producenta (poniżej). Atrapa żyje w `srp` (`core/radio/e220/sim`), tylko w testach; platforma `srp_platform` nie zna modemu.

## Dokument źródłowy (jedyny)

| | |
|---|---|
| Tytuł | **E220-xxxTxxx Series User Manual** (nagłówek stron: „E220-T Series User Manual”; strona tytułowa: „E220-xxxTxxx Product Specification Sheet — AT commands 20/30dBm LoRa wireless module”) |
| Wydawca | Chengdu Ebyte Electronic Technology Co., Ltd. |
| Wersja | **1.8, 2026-11-12** (historia zmian, str. 54), 54 strony |
| Źródło | `https://www.cdebyte.com/pdf-down.aspx?id=4221` — link „Manual” na stronie produktu `https://www.cdebyte.com/products/E220-400T30S/4` (zakładka Downloads) |
| Pobrano | 2026-11-16, kopia **poza repo** (prawa autorskie): `/home/matik/mavlink/e220doc/manual.pdf`, SHA-256 `e8028feaefe36a93c6f95f04958c5ae47d249aff3e4e90e234bae9114eb750fd` |

**Numery tabel:** instrukcja NIE numeruje tabel (są tylko „Figure N”), dlatego cytujemy **numery sekcji i stron** (stopka „… 17”).
Driver S1 powinien wskazać ten sam dokument i wersję w swoim README (uwaga QA/planisty pkt 15).

## Co atrapa odtwarza (sekcja instrukcji → zachowanie → test w `e220_sim_test.py`)

| Sekcja (str.) | Zachowanie |
|---|---|
| 5.1 (17) | tryby z pinów M1/M0: 00=transmisja, 01=WOR Tx, 10=WOR Rx, 11=sleep/konfiguracja. Tryb 0 — dane UART ↔ radio; tryb 2 — nadawanie wyłączone („WOR send closed”); tryb 3 — komendy HEX |
| 5.2.1 (24) | po zasilaniu AUX nisko ≈ 16 ms, potem wysoko |
| 5.2.4 (27–28) | przełączenie trybu: AUX nisko ≈ 10 ms (9–11 ms); gdy AUX nisko — piny nie są oceniane (zmiana stosowana po zwolnieniu); wyjście z trybu 3 resetuje parametry, AUX nisko dłużej; bajty UART podczas AUX nisko (zajęty) są ignorowane i liczone (`bytes_while_busy`) |
| 5.2.2 (24) | odbiór z radia: AUX nisko ≈ 3 ms przed wyjściem danych na TXD, potem wysoko |
| 5.2.3 (25–26) | rama kończy się po ciszy ≈ 3 bajtów (wg bauda UART); bufor > długość pakietu → nadawanie w pakietach |
| 6.1 (30–31) | komendy trybu 3 (9600 8N1): `C0` zapis trwały, `C1` odczyt, `C2` zapis tymczasowy; odpowiedź `C1 start len params`; błąd formatu → `FF FF FF`; przykłady z instrukcji są testami (`test_manual_example_*`) |
| 6.2/6.3 (31–34) | rejestry 00H–07H: ADDH/ADDL, REG0 (baud UART [7:5], parzystość [4:3], szybkość radiowa [2:0]), REG1 (długość pakietu [7:6], moc [1:0], bity RSSI/soft-switch przechowywane), REG2 (kanał), REG3 (bit7 bajt RSSI, bit6 fixed-point, LBT/WOR przechowywane), CRYPT 06H/07H tylko zapis (odczyt = 0) |
| 6.4 (35) | ustawienia fabryczne `00 00 62 00 17 03 00 00` (433,125 MHz, 9600 8N1, 2,4 kbps, 30 dBm) |
| 5.1.1.1–5.1.1.3 (17–21) | transparent: „co wyślesz, to odbierzesz”; fixed-point: pierwsze 3 B = adres H, adres L, kanał (nie trafiają do odbiorcy); broadcast adres FFFF |

Odwzorowanie uwag z review (pkt 15): wymuszenie REG3 *transparent* (bit6=0 — test `fixed_mode_consumes_three_byte_prefix…` pokazuje skutek
zostawienia fixed), bajt RSSI wyłączony (bit7 — `rssi_byte_appended…`), zmiana bauda po REG0 działa dopiero w trybie normalnym
(`uart_speed_after_reg0…`), AUX przed/po zmianie trybu (`commands_ignored_while_aux_low`, `pin_change_while_busy…`).
Prędkość UART jest czytana z termios pty (`set_uart_speed` na slave) — niezgodność = bajty ignorowane (`baud_mismatch_bytes`).

## Kontrakt sterowania (katalog `ctl_dir`)

`m0`, `m1` — zapisuje sterownik/test (`"0"`/`"1"`, najlepiej atomowo przez rename); `aux` — zapisuje atrapa (`"1"` = wolny);
`eeprom.bin` — 8 B rejestrów (usunięcie = ustawienia fabryczne). UART: `sim.slave_path` (pty). Statystyki: `sim.stats`
(`eeprom_writes`, `temp_writes`, `format_errors`, `baud_mismatch_bytes`, `bytes_while_busy`, `tx_packets`, `rx_packets`, …), log komend `sim.cmd_log`.
Idempotencja drivera (spec): drugi start bez zmian ⇒ `stats["eeprom_writes"] == 0`; zmiana jednego rejestru ⇒ zapis tylko tego rejestru
(odczyt `cmd_log`).

## Luki i błędy w dokumencie (atrapa NIE zgaduje — zachowanie konserwatywne)

1. **6.4 (str. 35):** „C0 00 09 …” — długość 09, ale wymienione jest 8 bajtów parametrów (00H–07H). Atrapa używa 8.
2. **6.3 REG0 [2:0]:** wartości 000, 001, 010 opisane wszystkie jako „2.4k” (010 = domyślne). Atrapa mapuje je na 2400 bps.
3. **Rejestr 08H (informacja o produkcie, 1 B):** instrukcja nie podaje wartości → odczyt zwraca `FF FF FF`, chyba że test poda `product_info=`.
4. Brak liczb dla czasu resetu po wyjściu z trybu 3 (sekcja 5.2.4 uwaga 3 mówi tylko „AUX nisko”): domyślnie dodatkowe 10 ms (`reset_ms`).

## INTERPRETACJE (instrukcja milczy; do potwierdzenia na prawdziwym module przez usera)

- Parametry zapisane `C2` (tymczasowo) przepadają po wyjściu z trybu 3 (reset wczytuje EEPROM).
- Niepełna komenda HEX porzucana po 100 ms ciszy; bajty komend w trakcie AUX nisko są ignorowane (nie kolejkowane).
- Filtrowanie adresów w transparent: odbiera, gdy adresy równe lub któryś = FFFF; ten sam kanał i szybkość radiowa. Fixed: adres docelowy = własny lub FFFF.
- Czas antenowy = bajty·8/szybkość radiowa (bez parametrów LoRa SF/BW); `Air(time_scale=0)` wyłącza opóźnienie.
- AUX przy nadawaniu: nisko od pierwszego bajtu do końca ramki UART; sam czas lotu nie jest odzwierciedlony (domyślne zachowanie wersji 7463-0-15+, sekcja 7.1.7).

## Bufor UART i AUX przy nadawaniu (sekcja 5.2.3, str. 25–26; 7.1.7, str. 38)

- Bufor UART **400 B** (n w tabeli parametrów 5.2.3). Dane ponad pojemność są **tracone** (`stats["tx_overflow"]` = liczba bajtów) —
  driver musi czekać na AUX; atrapa NIE blokuje nadawcy.
- Pakiety ≤ długość pakietu (REG1) wychodzą w tempie szybkości radiowej (model: bajty·8/szybkość, `Air(time_scale=…)` skraca czas w testach).
- Scenariusz 1 (dane ≤ długość pakietu): AUX nisko w trakcie odbioru z UART do przerwy ramki, potem wysoko (fw ≥ 7463-0-15 nie sygnalizuje lotu).
- Scenariusz 2 (dane > długość pakietu): AUX nisko aż **cały bufor zostanie wysłany** (≈ czas antenowy całości).
- `aux_indicates_air=True` (fw ≤ 7463-0-14): AUX nisko także podczas nadawania pojedynczego pakietu.
- INTERPRETACJA: kanał > 83 (pasmo 400 MHz, 6.3 REG2) — moduł nie nadaje ani nie odbiera (`stats["channel_out_of_range"]`).

## Poza zakresem atrapy

Kontrola parzystości UART (8O1/8E1 — bity REG0 [4:3] są tylko przechowywane, baud/format ramki UART nie jest weryfikowany poza prędkością), protokół WOR (tryby 1/2 tylko jako stany pinów), komendy AT (rozdz. 7), komendy `C0 C1 C2 C3` (RSSI/przełączanie programowe), LBT,
RSSI otoczenia, aktualizacja firmware (5.3). Bajt RSSI po danych (REG3 bit7) ma stałą wartość `rssi_byte` (domyślnie 0xC4).

## Para modułów (harness kontraktu `IRadioDriver`)

Atrapa daje „drugą stronę przewodu” bez sprzętu: para modułów A (badany przez `E220RadioDriver`) i B (zdalny) połączona radiem.

```sh
python3 core/radio/e220/sim/e220_sim.py --ctl-a /tmp/e220a --ctl-b /tmp/e220b --air-scale 0
# stdout: SLAVE_A=/dev/pts/X CTL_A=/tmp/e220a SLAVE_B=/dev/pts/Y CTL_B=/tmp/e220b
```

| Harness | Atrapa |
|---|---|
| `transport` | `E220RadioDriver` z UART = `SLAVE_A`, GPIO M0/M1/AUX = pliki `CTL_A/m0`, `m1`, `aux` |
| `peer_write(b)` | `write(SLAVE_B, b)` — B nadaje przez radio, A wypisuje na UART |
| `peer_read(want, t)` | `read(SLAVE_B)` — to, co A nadał przez radio |
| `overflow_bytes` | ≥ 1000 (bufor UART modułu 400 B → `tx_overflow`; sterownik ma czekać na AUX) |

B musi mieć ten sam kanał i szybkość radiową co A (`--b-regs`). `--air-scale 0` zeruje czas antenowy (testy funkcjonalne), `--air-scale 1` dla testów AUX/tempa.
Po `Open` driver ma zostawić A w trybie 0, z bajtem RSSI wyłączonym i transparentnym REG3 (inaczej dane mają dodatkowy bajt/prefiks — testy w `e220_sim_test.py`).
