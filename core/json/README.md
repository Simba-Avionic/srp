# json

Bezpieczny wrapper nad nlohmann/json do konfiguracji aplikacji.

## TL;DR

`JsonParser` ładuje plik lub obiekt JSON i zwraca `optional` przy brakującym kluczu — bez wyjątków na typowych ścieżkach odczytu.

## Opis funkcjonalności

- Parsowanie z ścieżki pliku albo z `nlohmann::json`.
- `GetObject` / `GetString` / `GetNumber<T>` / `GetArray<T>`.
- Używane przez engine (arm pins), servo, recovery, ADC sensor, env.

## API

```cpp
class JsonParser {
  static std::optional<JsonParser> Parser(const std::string& path) noexcept;
  static std::optional<JsonParser> Parser(nlohmann::json obj) noexcept;
  explicit JsonParser(const std::string& data);
  explicit JsonParser(nlohmann::json json);

  std::optional<JsonParser> GetObject(const std::string& name) const;
  nlohmann::json GetObject() const;
  std::optional<std::string> GetString(const std::string& name) const noexcept;
  template<typename T> std::optional<std::vector<T>> GetArray(const std::string& name) const;
  template<typename T> std::optional<T> GetNumber(const std::string& name) const noexcept;
};
```

## Zależności

`@com_json//:json` (nlohmann).

## BUILD

- `//core/json:simba_json`
- Pokrycie: `//core/ut:core_test`
