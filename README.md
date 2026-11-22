# srp

Dokumentacja modułów (nazwa, TL;DR, funkcje, diagramy, API):
- [core](core/README.md) — sterowniki
- [mw](mw/README.md) — middleware GPIO / I2C / temp / czas
- [apps](apps/README.md) — aplikacje EC / FC / SEC_EC

Konfigi do budowania (platforma + wersja plytki, definicje w `platforms/BUILD`):
- BBB + EB_V2
    ```
    --config=bbb-EB_V2
    ```
- BBB + LB_V1
    ```
    --config=bbb-LB_V1
    ```
- RPi Zero 2 W + NG_V1
    ```
    --config=rpi-NG_V1
    ```

Kod zalezny od plytki: `select({"//platforms:is_eb_v2": [...], "//conditions:default": [...]})`.

ostatnie zbudowane obrazy dostępne do pobrania pod adresem:
- [EC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/ec/pkg.tar)
- [FC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/fc/pkg.tar)

## Potrzebne wersje:
cpplint==1.6.0


## [Devboard tutorial](https://simbav.toadres.pl/pl/Dokumentacja/Hardware/DevBoard)