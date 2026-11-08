# tests/audit — testy QEMU i reprodukcje znanych błędów

Testy sprawdzają EC (pakiet `//deployment/cpu/ec:pkg` zbudowany w trybie `--define build_mode=sim`) na gościu QEMU z drivery sim (`core/sim`). Dwa rodzaje:

| Marker / tag | Co | Oczekiwany wynik |
|---|---|---|
| `functional` (pytest) | boot wszystkich aplikacji EC, ciśnienia/temperatura, serwo, logger, GPIO, primer, ścieżka LAUNCH | **zielone** (wymagane w CI) |
| `bugs` (pytest) i `audit_bugs` (UT, `cc_test`) | reprodukcje znalezisk `F-xx` — test sprawdza **poprawne** zachowanie | **czerwone**, dopóki błąd nie zostanie poprawiony; komunikat zawiera `F-xx` i `plik:linia` |

Kodu produkcyjnego te testy nie zmieniają. Gdy test `bugs` zacznie przechodzić, błąd został naprawiony — przenieś test do `functional` (albo usuń razem z naprawą).

## Jak uruchomić

Wymagania: `bazel`, `docker` (użytkownik w grupie `docker`), `/dev/net/tun`, ok. 1 GB RAM na gościa.

```bash
tools/qemu_test/qemu_test.sh functional        # zielone = kod wyjścia 0 (~15 min)
tools/qemu_test/qemu_test.sh bugs              # UT audit_bugs + pytest bugs; informacyjnie, kod 0
tools/qemu_test/qemu_test.sh all [--no_cache]
```

Skrypt sam buduje pakiet EC (sim) i obraz QEMU, startuje gościa w kontenerze (własna sieć `tap0` 192.168.10.4/24 → gość 192.168.10.51, więc kilka kontenerów może działać naraz), uruchamia testy, drukuje podsumowanie i sprząta kontenery. Wyniki: `qemu_test_results/` (JUnit XML, logi, dla `bugs` także `mapping.md`: F-xx → test → wynik).

Same UT reprodukujące błędy (bez QEMU):

```bash
bazel test --test_tag_filters=audit_bugs //...          # wszystkie mają paść z komunikatem F-xx
```

`tools/ut/unittest.sh` (a więc CI) **pomija** testy z tagiem `audit_bugs`.

## Zawartość

- `test_functional.py`, `test_bugs.py` — testy pytest (`audit_lib.py`: ssh do gościa, sterowanie driverami sim przez `$SRP_SIM_DIR/ctl`, UDS/DoIP, wywołania SOME/IP).
- `*/ut/test_audit_bugs.cc*` i analogiczne w `mw/…/tests` — UT `audit_bugs` (gtest, część z ASAN).
- `tools/qemu_test/` — runner Dockera (`qemu_docker.sh`, `docker/`), `check_apps.sh` (żywotność procesów EC), `make_mapping.py`.

## Uwagi

- Gość startuje z `-accel tcg,thread=multi`; obraz dysku jest tylko do odczytu (`snapshot=on`), zmiany znikają po wyłączeniu.
- `bugs` w pytest restartują gościa po teście (wątki sekwencji LAUNCH, `std::terminate` w EnvApp), dlatego trwają dłużej niż `functional`.
- Tylko jeden test (`fn02`) obserwuje żywotność aplikacji przez dłuższy czas; pozostałe czekają na warunek.

## CI

Workflow: `.github/workflows/qemu_tests.yml`. Woła wyłącznie `tools/qemu_test/qemu_test.sh [functional|bugs]`.

### Jak uruchomić

- **Label `qemu-tests` na PR** (do `master`): jeden bieg `qemu-functional`. Ponowny bieg: zdjąć i dodać label.
- **Ręcznie:** Actions → „QEMU tests” → Run workflow, `suite` = `functional` (domyślnie) | `bugs` | `all`.
- Nowy push do PR/brancha anuluje trwający bieg (concurrency per PR/ref). Brak biegu przy zwykłych commitach — to celowe (job ~20–45 min).

### Co jest informacyjne

- `qemu-bugs` (tylko dispatch `bugs|all`): `continue-on-error`, testy repro są **czerwone z założenia**; tabela F-xx w podsumowaniu joba.
- `qemu-functional`: powinien być zielony; artefakty (JUnit, logi) 7 dni.

### Required check (admin)

Check `QEMU functional` istnieje tylko po dodaniu labela, więc ustawienie go jako *required* zablokuje każdy PR bez labela (check „oczekuje” w nieskończoność). Opcje:
1. Zostawić opt-in (rekomendowane na start) — sprawdza reviewer przed merge.
2. Gdy przejdziemy na pełne triggery (`on: pull_request` bez `labeled`, ewentualnie `merge_group`), dodać w Settings → Rules → Rulesets (target `master`) „Require status checks” z `QEMU functional`.
