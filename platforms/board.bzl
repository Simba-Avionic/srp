"""Pliki konfiguracyjne zalezne od plytki (wersji PCB).

W pakiecie aplikacji / MW lezy jeden plik na plytke:
    config_BbbEbV2.json   - --config=bbb-EB_V2
    config_BbbLbV1.json   - --config=bbb-LB_V1
board_config() wybiera plik wybranej plytki i wydaje go jako config.json (tak go czyta aplikacja).
Bez wybranej plytki (build hosta, UT, build_all) brany jest BbbEbV2.
"""

# config_setting plytki -> nazwa plytki w nazwach plikow
BOARDS = {
    "//platforms:is_eb_v2": "BbbEbV2",
    "//platforms:is_lb_v1": "BbbLbV1",
}

DEFAULT_BOARD = "BbbEbV2"

def board_src(out, board):
    """Nazwa pliku zrodlowego plytki: config.json -> config_BbbEbV2.json."""
    stem, dot, ext = out.rpartition(".")
    if not dot:
        return out + "_" + board
    return stem + "_" + board + "." + ext

def board_file(out):
    """select() pliku zrodlowego wybranej plytki dla pliku wynikowego out."""
    branches = {setting: [board_src(out, board)] for setting, board in BOARDS.items()}
    branches["//conditions:default"] = [board_src(out, DEFAULT_BOARD)]
    return select(branches)

def board_config(name, out = "config.json", visibility = None):
    """Kopiuje <stem>_<Plytka>.<ext> wybranej plytki do out (domyslnie config.json).

    Wynik ma etykiete :<out> (np. :config.json), wiec trafia do pkg_tar (etcs) pod wlasciwa nazwa
    i moze byc uzywany jak dawny plik zrodlowy (np. data testow).
    """
    native.genrule(
        name = name,
        srcs = board_file(out),
        outs = [out],
        cmd = "cp $< $@",
        visibility = visibility,
    )
