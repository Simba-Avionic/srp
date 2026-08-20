# binary_file_writer

Zapis surowych bajtów do pliku binarnego.

## TL;DR

`BinaryFileWriter` otwiera plik w trybie `ios::binary` i dopisuje `vector<uint8_t>` z opcjonalnym flush. Prostszy odpowiednik CSV dla telemetrii binarnej.

## API

```cpp
namespace binaryWriter {
class BinaryFileWriter {
  bool open(const std::string& path);
  void close();
  bool write(const std::vector<uint8_t>& data, bool flush_after_write = true);
};
}
```

## Zależności

`ara/log` (kontekst `"bfwr"`).

## BUILD

- `//core/binary_file_writer:binary_file_lib`
