# srp

Dokumentacja modułów (nazwa, TL;DR, funkcje, diagramy, API):
- [core](core/README.md) — sterowniki
- [mw](mw/README.md) — middleware GPIO / I2C / temp / czas
- [apps](apps/README.md) — aplikacje EC / FC / SEC_EC

Konfigi do budowania:
- BBB
    ```
    --config=bbb-platform
    ```

ostatnie zbudowane obrazy dostępne do pobrania pod adresem:
- [EC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/ec/pkg.tar)
- [FC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/fc/pkg.tar)

## Potrzebne wersje:
cpplint==1.6.0


## [Devboard tutorial](https://simbav.toadres.pl/pl/Dokumentacja/Hardware/DevBoard)