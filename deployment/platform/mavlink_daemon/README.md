# MavlinkDaemon — deployment EC

Instancja MavlinkDaemona dla EC. Binarka i model pochodzą z `srp_platform`, a konfiguracja należy do deploymentu srp
(analogicznie do `someip_demon`): `adaptive_application` w `BUILD` pakuje `mavlink_daemon_config.json` i
`ebyte_e220_lora.json` do `opt/mavlink_daemon/`, a plugin `libebyte_e220.so` trafia do `opt/mavlink_daemon/drivers/`
(`driver_dir` z configu).

| Plik | Zawartość |
|------|-----------|
| `mavlink_daemon_config.json` | `system_id`/`component_id`, katalog driverów, łącze `lora` → driver `ebyte_e220` |
| `ebyte_e220_lora.json` | plik drivera E220: UART, piny M0/M1/AUX, kanał, moc, air rate (format: `core/radio/e220/README.md`) |

**Do potwierdzenia przez usera na sprzęcie EC:** numery GPIO M0/M1/AUX (`17`/`27`/`22`) i UART (`/dev/ttyS1`)
w `ebyte_e220_lora.json` są wartościami przykładowymi.

## Build i testy na Fedorze (stan 2026-11-18)

Komendy dla `//core/radio/e220/...` i `//deployment/cpu/ec:pkg` z platformą z lokalnego klonu
(`--override_repository=srp_platform=<klon srp_platform>`). Konfigurację środowiska podajemy w linii poleceń, nie w `.bazelrc`:

- **ccache:** wrapper `/usr/lib64/ccache/gcc` w sandboksie bazela kończy się `ccache: error: Read-only file system` →
  `--action_env=CCACHE_DISABLE=1`.
- **Python 3.12 dla pip_deps:** `lxml==5.4.0` i `fastcrc==0.3.2` nie mają kół dla Pythona 3.14. Utwórz venv
  (`python3.12 -m venv <katalog>`) i wstaw go na początek `PATH` bazela:
  `--repo_env=PATH=<katalog>/bin:/usr/bin:/bin --action_env=PATH=<to samo> --test_env=PATH=<to samo>`.
- **Submoduł:** `git -c protocol.file.allow=always submodule update --init deployment/system_definition`
  (bez niego analiza `//deployment/cpu/ec:pkg` kończy się brakiem pakietu `deployment/system_definition/diag/...`).
- **cpplint:** CI używa cpplint 1.x. Pip-owy cpplint 2.0.x zgłasza dodatkowe uwagi (np. `whitespace/indent_namespace`),
  więc do porównania z CI użyj `pip install cpplint==1.6.1` (w venv) i uruchom skrypt z `tools/cpplint/cpplint.sh`.
- **Ciężkie kroki:** `bazel build/test` pod `flock -o <lock>` i z `--jobs=8`; po fazie `bazel shutdown`.
