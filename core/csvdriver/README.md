# csvdriver

Zapis logów CSV (nagłówek + linie) przez wstrzykiwany `IFileHandler`.

## TL;DR

`CSVDriver` otwiera plik, zapisuje HEADER i kolejne linie. Separator (domyślnie `;`) jest przechowywany, ale `WriteLine` zapisuje linię as-is. Używany przez `logger_service` na EC i FC.

## Opis funkcjonalności

- `Init` — opcjonalny handler pliku (domyślnie `FileHandler`).
- `Open(fileName, HEADER)` — tworzy plik i zapisuje nagłówek.
- `WriteLine` — dopisanie wiersza, opcjonalny flush.
- Zwraca `0` OK / `-1` błąd.

## Architektura

```mermaid
flowchart LR
  Logger[logger_service] --> CSV[CSVDriver]
  CSV --> FH[IFileHandler]
  FH --> Disk[("/home/root/<ts>_log.csv")]
```

## API

```cpp
class ICSVDriver {
  virtual void Init(std::unique_ptr<IFileHandler> = std::make_unique<FileHandler>()) = 0;
  virtual int WriteLine(const std::string& line, bool flush_after_write = true) = 0;
  virtual int Open(const std::string& fileName, const std::string& HEADER) = 0;
  virtual void Close() = 0;
};

class CSVDriver : public ICSVDriver {
  explicit CSVDriver(const char& separator = ';');
};
```

## Zależności

`//core/file:file_lib`.

## BUILD

- `//core/csvdriver:csvdriver`
- `//core/csvdriver/mock:mock_csvdriver`
- Test: `//core/csvdriver/ut:csv_test`
