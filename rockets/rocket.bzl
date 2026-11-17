"""Pliki zalezne od rakiety (R8 / Rurku).

Rakieta to flaga builda (nie platforma): --config=r8 / --config=rurku (.bazelrc: --define rocket=...).
W pakiecie lezy jeden plik na rakiete, np.:
    engine_app_R8.cpp, engine_app_Rurku.cpp      - rocket_file("engine_app.cpp") w srcs
    config_R8.json,    config_Rurku.json         - rocket_config() -> config.json
Bez flagi rakiety (build hosta, UT, build_all) brany jest R8.
"""

# config_setting rakiety -> nazwa rakiety w nazwach plikow
ROCKETS = {
    "//rockets:is_r8": "R8",
    "//rockets:is_rurku": "Rurku",
}

DEFAULT_ROCKET = "R8"

def rocket_src(out, rocket):
    """Nazwa pliku zrodlowego rakiety: engine_app.cpp -> engine_app_R8.cpp."""
    stem, dot, ext = out.rpartition(".")
    if not dot:
        return out + "_" + rocket
    return stem + "_" + rocket + "." + ext

def rocket_file(out):
    """select() pliku zrodlowego wybranej rakiety, np. srcs = [...] + rocket_file("engine_app.cpp")."""
    branches = {setting: [rocket_src(out, rocket)] for setting, rocket in ROCKETS.items()}
    branches["//conditions:default"] = [rocket_src(out, DEFAULT_ROCKET)]
    return select(branches)

def rocket_config(name, out = "config.json", visibility = None):
    """Kopiuje <stem>_<Rakieta>.<ext> wybranej rakiety do out (domyslnie config.json)."""
    native.genrule(
        name = name,
        srcs = rocket_file(out),
        outs = [out],
        cmd = "cp $< $@",
        visibility = visibility,
    )
