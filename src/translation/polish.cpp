#include "translation/common.h"
#include "translation/translation.h"

static translation_string all_strings[]
  = {{TR_NO_PATCH_TITLE, "Patch 1.0.1.0 nie jest zainstalowany"},
     {TR_NO_PATCH_MESSAGE,
      "Twoja instalacja Cezara 3 nie ma zainstalowanej łatki 1.0.1.0."
      "Łatkę możesz pobrać z:\n." URL_PATCHES "\n"
      "Kontynuujesz na własne ryzyko."},
     {TR_MISSING_FONTS_TITLE, "Brakuje czcionek"},
     {TR_MISSING_FONTS_MESSAGE,
      "Twoja instalacja Cezara 3 wymaga dodatkowych czcionek."
      "Możesz je pobrać dla własnego języka z:\n" URL_PATCHES},
     {TR_NO_EDITOR_TITLE, "Edytor nie jest zainstalowany"},
     {TR_NO_EDITOR_MESSAGE,
      "Twoja instalacja Cezara 3 nie ma plików edytora"
      "Możesz je pobrać z:\n" URL_EDITOR},
     {TR_INVALID_LANGUAGE_TITLE, "Nieprawidłowy folder językowy"},
     {TR_INVALID_LANGUAGE_MESSAGE,
      "Wybrany folder nie zawiera poprawnego pakietu językowego"
      "Proszę sprawdzić błędy w pliku log"},
     {TR_BUTTON_OK, "OK"},
     {TR_BUTTON_CANCEL, "Anuluj"},
     {TR_BUTTON_RESET_DEFAULTS, "Ustaw domyślne"},
     {TR_BUTTON_CONFIGURE_HOTKEYS, "Skróty klawiszowe"},
     {TR_CONFIG_TITLE, "konfiguracja"},
     {TR_CONFIG_LANGUAGE_LABEL, "Język:"},
     {TR_CONFIG_LANGUAGE_DEFAULT, "(domyślny)"},
     {TR_CONFIG_HEADER_UI_CHANGES, "Zmiany interfejsu użytkownika"},
     {TR_CONFIG_HEADER_GAMEPLAY_CHANGES, "Zmiany gry"},
     {TR_CONFIG_HEADER_GODS_CHANGES, "Gods changes"},
     {TR_CONFIG_SHOW_INTRO_VIDEO, "Odgrywaj film wprowadzający"},
     {TR_CONFIG_SIDEBAR_INFO, "Dodatkowe informacje w panelu sterowania"},
     {TR_CONFIG_SMOOTH_SCROLLING, "Płynne przewijanie"},
     {TR_CONFIG_VISUAL_FEEDBACK_ON_DELETE, "Pokaż wybrany obszar przy oczyszczaniu terenu"},
     {TR_CONFIG_ALLOW_CYCLING_TEMPLES, "Pozwól na budowę kolejno każdej ze świątyń"},
     {TR_CONFIG_SHOW_WATER_STRUCTURE_RANGE, "Pokaż zasięg przy budowie rezerwuarów, fontann i studni"},
     {TR_CONFIG_SHOW_CONSTRUCTION_SIZE, "Pokaż rozmiar budowy przy przeciąganiu kursorem"},
     {TR_CONFIG_HIGHLIGHT_LEGIONS, "Podświetl legion przy najechaniu kursorem"},
     {TR_CONFIG_FIX_IMMIGRATION_BUG, "Napraw błąd z imigrację na najwyższym poziomie trudności"},
     {TR_CONFIG_FIX_100_YEAR_GHOSTS, "Napraw 100-letnie duchy"},
     {TR_CONFIG_FIX_EDITOR_EVENTS, "Napraw zmianę imperatora i czas przetrwania w konstruktorze miast"},
     {TR_HOTKEY_TITLE, "Konfiguracja skrótów klawiszowych"},
     {TR_HOTKEY_LABEL, "Skrót klawiszowy"},
     {TR_HOTKEY_ALTERNATIVE_LABEL, "Alternatywny"},
     {TR_HOTKEY_HEADER_ARROWS, "Strzałki kierunkowe"},
     {TR_HOTKEY_HEADER_GLOBAL, "Globalne skróty klawiszowe"},
     {TR_HOTKEY_HEADER_CITY, "Miejskie skróty klawiszowe"},
     {TR_HOTKEY_HEADER_ADVISORS, "Doradcy"},
     {TR_HOTKEY_HEADER_OVERLAYS, "Widoki"},
     {TR_HOTKEY_HEADER_BOOKMARKS, "Miejskie miejsca skoku"},
     {TR_HOTKEY_HEADER_EDITOR, "Edytor"},
     {TR_HOTKEY_ARROW_UP, "Góra"},
     {TR_HOTKEY_ARROW_DOWN, "Dół"},
     {TR_HOTKEY_ARROW_LEFT, "Lewo"},
     {TR_HOTKEY_ARROW_RIGHT, "Prawo"},
     {TR_HOTKEY_TOGGLE_FULLSCREEN, "Przełącz pełny ekran"},
     {TR_HOTKEY_CENTER_WINDOW, "Wycentruj okno"},
     {TR_HOTKEY_RESIZE_TO_640, "Zmień rozmiar okna do 640x480"},
     {TR_HOTKEY_RESIZE_TO_800, "Zmień rozmiar okna do 800x600"},
     {TR_HOTKEY_RESIZE_TO_1024, "Zmień rozmiar okna do 1024x768"},
     {TR_HOTKEY_SAVE_SCREENSHOT, "Zapisz zrzut ekranu"},
     {TR_HOTKEY_SAVE_CITY_SCREENSHOT, "Zapisz zrzut ekranu miasta"},
     {TR_HOTKEY_LOAD_FILE, "Wczytaj plik"},
     {TR_HOTKEY_SAVE_FILE, "Zapisz plik"},
     {TR_HOTKEY_INCREASE_GAME_SPEED, "Zwiększ prędkość gry"},
     {TR_HOTKEY_DECREASE_GAME_SPEED, "Zmniejsz prędkość gry"},
     {TR_HOTKEY_TOGGLE_PAUSE, "Pauza"},
     {TR_HOTKEY_CYCLE_LEGION, "Przełącz między legionami"},
     {TR_HOTKEY_ROTATE_MAP_LEFT, "Obróć mapę w lewo"},
     {TR_HOTKEY_ROTATE_MAP_RIGHT, "Obróć mapę w prawo"},
     {TR_HOTKEY_SHOW_ADVISOR_LABOR, "Doradca zatrudnienia"},
     {TR_HOTKEY_SHOW_ADVISOR_MILITARY, "Doradca legionów"},
     {TR_HOTKEY_SHOW_ADVISOR_IMPERIAL, "Doradca imperialny"},
     {TR_HOTKEY_SHOW_ADVISOR_RATINGS, "Doradca wyników"},
     {TR_HOTKEY_SHOW_ADVISOR_TRADE, "Doradca handlowy"},
     {TR_HOTKEY_SHOW_ADVISOR_POPULATION, "Doradca populacji"},
     {TR_HOTKEY_SHOW_ADVISOR_HEALTH, "Doradca zdrowia"},
     {TR_HOTKEY_SHOW_ADVISOR_EDUCATION, "Doradca edukacji"},
     {TR_HOTKEY_SHOW_ADVISOR_ENTERTAINMENT, "Doradca rozrywki"},
     {TR_HOTKEY_SHOW_ADVISOR_RELIGION, "Doradca religijny"},
     {TR_HOTKEY_SHOW_ADVISOR_FINANCIAL, "Doradca finansowy"},
     {TR_HOTKEY_SHOW_ADVISOR_CHIEF, "Główny doradca"},
     {TR_HOTKEY_TOGGLE_OVERLAY, "Przełącz między widokami"},
     {TR_HOTKEY_SHOW_OVERLAY_WATER, "Pokaż widok wody"},
     {TR_HOTKEY_SHOW_OVERLAY_FIRE, "Pokaż widok pożarów"},
     {TR_HOTKEY_SHOW_OVERLAY_DAMAGE, "Pokaż widok zniszczenia"},
     {TR_HOTKEY_SHOW_OVERLAY_CRIME, "Pokaż widok zbrodni"},
     {TR_HOTKEY_SHOW_OVERLAY_PROBLEMS, "Pokaż widok problemów"},
     {TR_HOTKEY_GO_TO_BOOKMARK_1, "Przejdź do punktu skoku 1"},
     {TR_HOTKEY_GO_TO_BOOKMARK_2, "Przejdź do punktu skoku 2"},
     {TR_HOTKEY_GO_TO_BOOKMARK_3, "Przejdź do punktu skoku 3"},
     {TR_HOTKEY_GO_TO_BOOKMARK_4, "Przejdź do punktu skoku 4"},
     {TR_HOTKEY_SET_BOOKMARK_1, "Ustaw punkt skoku 1"},
     {TR_HOTKEY_SET_BOOKMARK_2, "Ustaw punkt skoku 2"},
     {TR_HOTKEY_SET_BOOKMARK_3, "Ustaw punkt skoku 3"},
     {TR_HOTKEY_SET_BOOKMARK_4, "Ustaw punkt skoku 4"},
     {TR_HOTKEY_EDITOR_TOGGLE_BATTLE_INFO, "Przełącz informacje o bitwach"},
     {TR_HOTKEY_EDIT_TITLE, "Wprowadź skrót"},
     {TR_BUILDING_ROADBLOCK, "Blokada drogi"},
     {TR_BUILDING_ROADBLOCK_DESC, "Blokada drogi zatrzymuje przechodniów bez celu."},
     {TR_HEADER_HOUSING, "Domy"},
     {TR_ADVISOR_HOUSING_ROOM, "W miejskich domach jest miejsce dla"},
     {TR_ADVISOR_HOUSING_NO_ROOM, "Brak miejsca w miejskich domach."},
     {TR_ADVISOR_RESIDENCES_DEMANDING_POTTERY, "Domy wymagające garnków"},
     {TR_ADVISOR_RESIDENCES_DEMANDING_FURNITURE, "Domy wymagające mebli"},
     {TR_ADVISOR_RESIDENCES_DEMANDING_OIL, "Domy wymagające oliwy"},
     {TR_ADVISOR_RESIDENCES_DEMANDING_WINE, "Domy wymagające wina"},
     {TR_ADVISOR_TOTAL_NUM_HOUSES, "Razem domów:"},
     {TR_ADVISOR_AVAILABLE_HOUSING_CAPACITY, "Dostępne zakwaterowanie:"},
     {TR_ADVISOR_TOTAL_HOUSING_CAPACITY, "Całkowite zakwaterowanie:"},
     {TR_ADVISOR_ADVISOR_HEADER_HOUSING, "Populacja - domy"},
     {TR_ADVISOR_BUTTON_GRAPHS, "Wykresy"},
     {TR_ADVISOR_HOUSING_PROSPERITY_RATING, "Bogactwo domów"},
     {TR_ADVISOR_PERCENTAGE_IN_VILLAS_PALACES, "Procent populacji w willach i pałacach"},
     {TR_ADVISOR_PERCENTAGE_IN_TENTS_SHACKS, "Procent populacji w namiotach i szałasach"},
     {TR_ADVISOR_AVERAGE_TAX, "Średni podatek na głowę"},
     {TR_ADVISOR_AVERAGE_AGE, "Średnia wieku populacji"},
     {TR_ADVISOR_PERCENT_IN_WORKFORCE, "Procent populacji w sile roboczej"},
     {TR_ADVISOR_BIRTHS_LAST_YEAR, "Narodzin w zeszłym roku:"},
     {TR_ADVISOR_DEATHS_LAST_YEAR, "Śmierci w zeszłym roku:"},
     {TR_ADVISOR_TOTAL_POPULATION, "Razem mieszkańców"}};

void translation_polish(const translation_string** strings, int* num_strings) {
    *strings = all_strings;
    *num_strings = sizeof(all_strings) / sizeof(translation_string);
}
