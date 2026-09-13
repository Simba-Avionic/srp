# time

Narzędzie do odczytu i ustawiania zegara systemowego przez polecenie `date`.

## TL;DR

`TimeChanger` wywołuje `date` / `popen`. Target Bazel jest oznaczony jako deprecated (`"you realy need to use this?"`). Do wspólnej osi czasu lotu używaj `timestamp`.

## Opis funkcjonalności

- Ustawienie czasu stringiem `MMDDhhmmYYYY.ss` albo strukturą `Date_t`.
- Odczyt aktualnego czasu systemowego.

## API

```cpp
struct Date_t {
  uint16_t year;
  uint8_t month, day, hour, minute, seconds;
};

class TimeChanger {
  static ErrorCode ChangeSystemTime(std::string time);
  static ErrorCode ChangeSystemTime(Date_t date);
  static std::optional<Date_t> ReadSystemTime();
  static std::optional<std::string> ReadSystemTimeAsString();
};
```

## Zależności

Shell `date`. Nie używa `timestamp` SHM.

## BUILD

- `//core/time:sys_time_change_lib`
- `//core/time/mock:mock_sys_time_changer`
