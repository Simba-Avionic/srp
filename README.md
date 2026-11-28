# srp

Dokumentacja modułów (nazwa, TL;DR, funkcje, diagramy, API):
- [core](core/README.md) — sterowniki
- [mw](mw/README.md) — middleware GPIO / I2C / temp / czas
- [apps](apps/README.md) — aplikacje EC / FC / SEC_EC

Konfigi do budowania (platforma + wersja plytki, definicje w `platforms/BUILD`):
- BBB + EB_V2
    ```
    --config=EB_V2
    ```
- BBB + LB_V1
    ```
    --config=LB_V1
    ```
- RPi Zero 2 W + UB_V1
    ```
    --config=UB_V1
    ```

Kod zalezny od plytki: `select({"//platforms:is_eb_v2": [...], "//conditions:default": [...]})`.

Pliki zalezne od plytki (mapowanie pinow, kanaly, sciezki urzadzen) leza obok BUILD jako `config_BbbEbV2.json`,
`config_BbbLbV1.json`; `board_config()` z `//platforms:board.bzl` wybiera plik wg `--config` i wydaje go
jako `config.json` (bez wybranej plytki - `BbbEbV2`).

Rakieta (flaga builda, niezalezna od plytki): `--config=r8` / `--config=rurku`, np.
`bazel build --config=bbb-EB_V2 --config=rurku //deployment/cpu/ec:pkg`. Pliki zalezne od rakiety leza obok BUILD
jako `<nazwa>_R8.<ext>` / `<nazwa>_Rurku.<ext>` i wybiera je `//rockets:rocket.bzl`: `rocket_file()` w `srcs`
(np. reakcje silnika `engine_reactions_R8.cpp`), `rocket_config()` dla configu (np. EnvApp `config_R8.json` -> `config.json`).
Bez flagi rakiety - `R8`.

ostatnie zbudowane obrazy dostępne do pobrania pod adresem:
- [EC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/ec/pkg.tar)
- [FC](https://simbaj.toadres.pl/job/srp-master-build/lastSuccessfulBuild/artifact/bazel-bin/deployment/cpu/fc/pkg.tar)

## Potrzebne wersje:
cpplint==1.6.0


## [Devboard tutorial](https://simbav.toadres.pl/pl/Dokumentacja/Hardware/DevBoard)