#pragma once

#include "graphics/elements/generic_button.h"
#include "io/dir.h"
#include "io/file.h"
#include "scrollbar.h"

#include <vector>

#define MAX_BUTTONS_IN_SCROLLABLE_LIST 50
#define MAX_MANUAL_ENTRIES 300

struct scrollable_list_ui_params {
    int x = 0;
    int y = 0;
    int blocks_x = 10;
    int blocks_y = 10;
    int buttons_size_x = -1; // default: defined by paneling
    int buttons_size_y = 16;
    int buttons_margin_x = 2; // padding of buttons inside the paneling
    int buttons_margin_y = 10;
    int text_padding_x = 6; // padding of text inside the buttons
    int text_padding_y = 0;
    int text_max_width = -1;    // for text ellipsing
    bool text_centered = false; // text centered inside padding/button
    int scrollbar_margin_x = 0;
    int scrollbar_margin_top = 0;
    int scrollbar_margin_bottom = 0;
    int scrollbar_dot_padding = 0;
    bool thin_scrollbar = false;
    bool draw_scrollbar_always = false;
    bool draw_paneling = true;
    font_t font_asleep = FONT_NORMAL_BLACK_ON_DARK;
    font_t font_focus = FONT_NORMAL_YELLOW;
    font_t font_selected = FONT_NORMAL_WHITE_ON_DARK;
};

enum {
    FILE_NO_EXT,
    FILE_WITH_EXT,
    FILE_FULL_PATH,
};

class scroll_list_panel {
private:
    generic_button list_buttons[MAX_BUTTONS_IN_SCROLLABLE_LIST] = {};
    int num_total_entries = 0;
    int num_buttons;
    int focus_button_id = 0;     // first valid --> 1
    int selected_entry_idx = -1; // first valid --> 0
    void (*left_click_callback)(int param1, int param2);
    void (*right_click_callback)(int param1, int param2);
    void (*double_click_callback)(int param1, int param2);
    void (*focus_change_callback)(int param1, int param2);

    scrollbar_type scrollbar;

    const dir_listing* file_finder;
    char files_dir[MAX_FILE_NAME];
    char files_ext[MAX_FILE_NAME];
    bool using_file_finder;

    char manual_entry_list[MAX_FILE_NAME][MAX_MANUAL_ENTRIES];

    void (*custom_text_render)(int button_index, const uint8_t* text, int x, int y, font_t font);
    bool using_custom_text_render = false;

    bool WAS_DRAWN = false; // for frame-ordered caching logic purposes

public:
    scrollable_list_ui_params ui_params;

    void select(const char* button_text);
    void select_by_button_id(int button_id);
    void select_entry(int entry_idx);
    void unselect();
    void unfocus();
    int get_focused_button_id();
    int get_selected_button_id();
    int get_focused_entry_idx();
    int get_selected_entry_idx();
    int get_total_entries();
    const char* get_entry_text_by_idx(int index, int filename_syntax);
    const char* get_selected_entry_text(int filename_syntax);
    int get_entry_idx(const char* button_text);
    bool has_entry(const char* button_text);

    void set_file_finder_usage(bool use);
    void clear_entry_list();
    void add_entry(const char* entry_text);
    //    void remove_entry(const char *entry_text); // TODO: nope.
    //    void remove_entry(int index); // TODO: nope.
    void change_file_path(const char* dir, const char* ext = nullptr);
    void append_files_with_extension(const char* dir, const char* ext);
    void refresh_file_finder();
    void refresh_scrollbar();
    void clamp_scrollbar_position();

    void draw();
    int input_handle(const mouse* m);

    scroll_list_panel(int n_buttons,
                      void (*lmb)(int param1, int param2),
                      void (*rmb)(int param1, int param2),
                      void (*dmb)(int param1, int param2),
                      void (*fcc)(int param1, int param2),
                      scrollable_list_ui_params params,
                      bool use_file_finder,
                      const char* dir = ".",
                      const char* ext = "");
    ~scroll_list_panel();
};
