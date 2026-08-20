# temp

Sterownik czujników temperatury 1-Wire (typ DS18B20) przez sysfs w1.

## TL;DR

Czyta temperaturę z `/sys/bus/w1/devices/<id>/`, ustawia rozdzielczość i listuje dostępne czujniki. Middleware `mw/temp` serializuje dostęp i pushuje odczyty do aplikacji.

## Opis funkcjonalności

- `ReadTemp` — wartość w °C (millicelsius / 1000). Ostrzega przy 85 °C i −127 °C (wartości power-on).
- `SetResolution` / `ReadResolution` — 9–12 bitów.
- `GetAllAccessibleSensor` — lista ID, pomija `w1_bus_master1`.
- `GetResponseTime` — czas konwersji (`conv_time`).

## Architektura

```mermaid
flowchart LR
  MW[mw::TempService] --> D[TempDriver]
  D --> W1["/sys/bus/w1/devices/28-xxxxxxxxxxxx"]
```

## API

```cpp
class ITempDriver {
  virtual Result<double> ReadTemp(const std::string& sensorPhysicalID) = 0;
  virtual Result<bool> SetResolution(const std::string& id, uint8_t res) = 0;
  virtual Result<uint8_t> ReadResolution(const std::string& id) = 0;
  virtual Result<std::vector<std::string>> GetAllAccessibleSensor() = 0;
  virtual Result<uint16_t> GetResponseTime(const std::string& id) = 0;
};

class TempDriver : public ITempDriver;
class MockTempDriver;
```

Błędy: `ara::core::Result` (`ara::com::ComErrc::kErroneousFileHandle` przy problemie z plikiem).

## Symulacja

`SIM_temp_driver`: stałe 33.33 °C, rozdzielczość 9, pusta lista czujników, czas odpowiedzi 250 ms.

## BUILD

- `//core/temp:temp_interface`
- `//core/temp:temp_driver`
- `//core/temp:SIM_temp_driver`
- `//core/temp/mock:mock_temp_driver`
