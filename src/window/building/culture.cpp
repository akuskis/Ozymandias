#include "culture.h"

#include "building/building.h"
#include "graphics/boilerplate.h"
#include "graphics/elements/lang_text.h"
#include "graphics/elements/panel.h"
#include "io/gamefiles/lang.h"

static void draw_building_service_info(building_info_context* c, int help_id, const char* sound_file, int group_id, e_figure_type ftype) {
    c->help_id = help_id;
    window_building_play_sound(c, sound_file);
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(group_id, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);

    building *b = building_get(c->building_id);

    if (ftype != FIGURE_NONE && b->has_figure_of_type(BUILDING_SLOT_SERVICE, ftype)) {
        window_building_draw_description(c, group_id, e_text_figure_on_patrol);
    } else if (!c->has_road_access) {
        window_building_draw_description(c, e_text_building, e_text_building_no_roads);
    } else if (building_get(c->building_id)->num_workers <= 0) {
        window_building_draw_description(c, group_id, e_text_no_workers);
    } else {
        window_building_draw_description(c, group_id, e_text_works_fine);
    }
    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 4);
    window_building_draw_employment(c, 142);
}

void window_building_draw_apothecary(building_info_context* c) {
    draw_building_service_info(c, 63, "wavs/apothecary.wav", e_text_building_apothecary, FIGURE_APOTHECARY);
}
void window_building_draw_dentist(building_info_context* c) {
    draw_building_service_info(c, 65, "wavs/dentist.wav", e_text_building_dentist, FIGURE_DENTIST);
}
void window_building_draw_mortuary(building_info_context* c) {
    draw_building_service_info(c, 66, "wavs/mortuary.wav", e_text_building_mortuary, FIGURE_MORTUARY_WORKER);
}
void window_building_draw_physician(building_info_context *c) {
    c->help_id = 64;
    window_building_play_sound(c, "wavs/physician.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(e_text_building_physician, e_text_title, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);

    building *b = building_get(c->building_id);
    //if (!b->has_water_access)
    //    window_building_draw_description(c, 83, 4);

    if (b->has_figure_of_type(BUILDING_SLOT_SERVICE, FIGURE_PHYSICIAN)) {
        window_building_draw_description(c, e_text_building_physician, e_text_figure_on_patrol);
    } else if (!c->has_road_access) {
        window_building_draw_description(c, e_text_building, e_text_building_no_roads);
    } else if (b->num_workers <= 0) {
        window_building_draw_description(c, e_text_building_physician, e_text_no_workers);
    } else {
        window_building_draw_description(c, e_text_building_physician, e_text_works_fine);
    }

    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 4);
    window_building_draw_employment(c, 142);
}

void window_building_draw_school(building_info_context* c) {
    draw_building_service_info(c, 68, "wavs/school.wav", 85, FIGURE_SCHOOL_CHILD);
}
void window_building_draw_academy(building_info_context* c) {
    draw_building_service_info(c, 69, "wavs/academy.wav", 86, FIGURE_TEACHER);
}
void window_building_draw_library(building_info_context* c) {
    draw_building_service_info(c, 70, "wavs/library.wav", 87, FIGURE_LIBRARIAN);
}

static void draw_temple(building_info_context* c, const char* sound_file, int group_id, int image_offset) {
    c->help_id = 67;
    window_building_play_sound(c, sound_file);
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(group_id, 0, c->x_offset, c->y_offset + 12, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    inner_panel_draw(c->x_offset + 16, c->y_offset + 56, c->width_blocks - 2, 4);
    window_building_draw_employment(c, 62);
    if (c->has_road_access) {
        ImageDraw::img_generic(image_offset + image_id_from_group(GROUP_PANEL_WINDOWS), c->x_offset + 190, c->y_offset + 16 * c->height_blocks - 118);
    } else {
        window_building_draw_description_at(c, 16 * c->height_blocks - 128, 69, 25);
    }
}
static void draw_shrine(building_info_context* c, const char* sound_file, int text_id, int image_offset) {
    c->help_id = 67;
    window_building_play_sound(c, sound_file);
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(161, text_id, c->x_offset, c->y_offset + 12, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);

    if (c->has_road_access) {
        ImageDraw::img_generic(image_offset + image_id_from_group(GROUP_PANEL_WINDOWS), c->x_offset + 190, c->y_offset + 16 * c->height_blocks - 148);
    } else {
        window_building_draw_description_at(c, 16 * c->height_blocks - 128, 69, 25);
    }
}

void window_building_draw_temple_osiris(building_info_context* c) {
    draw_temple(c, "wavs/temple_farm.wav", 92, 21);
}
void window_building_draw_temple_ra(building_info_context* c) {
    draw_temple(c, "wavs/temple_ship.wav", 93, 22);
}
void window_building_draw_temple_ptah(building_info_context* c) {
    draw_temple(c, "wavs/temple_comm.wav", 94, 23);
}
void window_building_draw_temple_seth(building_info_context* c) {
    draw_temple(c, "wavs/temple_war.wav", 95, 24);
}
void window_building_draw_temple_bast(building_info_context* c) {
    draw_temple(c, "wavs/temple_love.wav", 96, 25);
}

void window_building_draw_shrine_osiris(building_info_context* c) {
    draw_shrine(c, "wavs/temple_farm.wav", 0, 21);
}
void window_building_draw_shrine_ra(building_info_context* c) {
    draw_shrine(c, "wavs/temple_ship.wav", 2, 22);
}
void window_building_draw_shrine_ptah(building_info_context* c) {
    draw_shrine(c, "wavs/temple_comm.wav", 4, 23);
}
void window_building_draw_shrine_seth(building_info_context* c) {
    draw_shrine(c, "wavs/temple_war.wav", 6, 24);
}
void window_building_draw_shrine_bast(building_info_context* c) {
    draw_shrine(c, "wavs/temple_love.wav", 8, 25);
}

void window_building_draw_oracle(building_info_context* c) {
    c->help_id = 67;
    window_building_play_sound(c, "wavs/oracle.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(110, 0, c->x_offset, c->y_offset + 12, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    window_building_draw_description_at(c, 16 * c->height_blocks - 158, 110, 1);
}

void window_building_draw_booth(building_info_context* c) {
    c->help_id = 71;
    window_building_play_sound(c, "wavs/theatre.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(72, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    building* b = building_get(c->building_id);
    if (!c->has_road_access) {
        window_building_draw_description(c, 69, 25);
    } else if (b->num_workers <= 0) {
        window_building_draw_description(c, 72, 4);
    } else if (!b->data.entertainment.num_shows) {
        window_building_draw_description(c, 72, 2);
    } else if (b->data.entertainment.days1) {
        window_building_draw_description(c, 72, 3);
    }

    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 6);
    window_building_draw_employment(c, 138);
    if (b->data.entertainment.days1 > 0) {
        int width = lang_text_draw(72, 6, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days1, c->x_offset + width + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw(72, 7 + b->data.entertainment.days3_or_play, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(72, 5, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
    }
}
void window_building_draw_bandstand(building_info_context* c) {
    c->help_id = 72;
    // The file name has a typo
    window_building_play_sound(c, "wavs/ampitheatre.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(71, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    building* b = building_get(c->building_id);
    if (!c->has_road_access)
        window_building_draw_description(c, 69, 25);
    else if (b->num_workers <= 0)
        window_building_draw_description(c, 71, 6);
    else if (!b->data.entertainment.num_shows)
        window_building_draw_description(c, 71, 2);
    else if (b->data.entertainment.num_shows == 2)
        window_building_draw_description(c, 71, 3);
    else if (b->data.entertainment.days1)
        window_building_draw_description(c, 71, 4);
    else if (b->data.entertainment.days2)
        window_building_draw_description(c, 71, 5);

    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 7);
    window_building_draw_employment(c, 138);
    if (b->data.entertainment.days1 > 0) {
        int width = lang_text_draw(71, 8, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days1, c->x_offset + width + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(71, 7, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
    }
    if (b->data.entertainment.days2 > 0) {
        int width = lang_text_draw(71, 10, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days2, c->x_offset + width + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw(72, 7 + b->data.entertainment.days3_or_play, c->x_offset + 32, c->y_offset + 222, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(71, 9, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    }
}
void window_building_draw_pavilion(building_info_context* c) {
    c->help_id = 73;
    window_building_play_sound(c, "wavs/colloseum.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(74, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    building* b = building_get(c->building_id);

    if (!c->has_road_access)
        window_building_draw_description(c, 69, 25);
    else if (b->num_workers <= 0)
        window_building_draw_description(c, 74, 6);
    else if (!b->data.entertainment.num_shows)
        window_building_draw_description(c, 74, 2);
    else if (b->data.entertainment.num_shows == 2)
        window_building_draw_description(c, 74, 3);
    else if (b->data.entertainment.days1)
        window_building_draw_description(c, 74, 5);
    else if (b->data.entertainment.days2)
        window_building_draw_description(c, 74, 4);

    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 6);
    window_building_draw_employment(c, 138);
    if (b->data.entertainment.days1 > 0) {
        int width = lang_text_draw(74, 8, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days1, c->x_offset + width + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(74, 7, c->x_offset + 32, c->y_offset + 182, FONT_NORMAL_BLACK_ON_DARK);
    }
    if (b->data.entertainment.days2 > 0) {
        int width = lang_text_draw(74, 10, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days2, c->x_offset + width + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(74, 9, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    }
}
void window_building_draw_senet_house(building_info_context* c) {
    c->help_id = 74;
    window_building_play_sound(c, "wavs/hippodrome.wav");
    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(73, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    building* b = building_get(c->building_id);
    if (!c->has_road_access)
        window_building_draw_description(c, 69, 25);
    else if (b->num_workers <= 0)
        window_building_draw_description(c, 73, 4);
    else if (!b->data.entertainment.num_shows)
        window_building_draw_description(c, 73, 2);
    else if (b->data.entertainment.days1)
        window_building_draw_description(c, 73, 3);

    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 6);
    window_building_draw_employment(c, 138);
    if (b->data.entertainment.days1 > 0) {
        int width = lang_text_draw(73, 6, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
        lang_text_draw_amount(8, 44, 2 * b->data.entertainment.days1, c->x_offset + width + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    } else {
        lang_text_draw(73, 5, c->x_offset + 32, c->y_offset + 202, FONT_NORMAL_BLACK_ON_DARK);
    }
}

static void draw_entertainment_school(building_info_context* c, const char* sound_file, int group_id) {
    c->help_id = 75;
    window_building_play_sound(c, sound_file);

    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(group_id, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
    if (!c->has_road_access) {
        window_building_draw_description(c, 69, 25);
    } else if (building_get(c->building_id)->num_workers <= 0) {
        window_building_draw_description(c, group_id, 7);
    } else if (c->worker_percentage >= 100) {
        window_building_draw_description(c, group_id, 2);
    } else if (c->worker_percentage >= 75) {
        window_building_draw_description(c, group_id, 3);
    } else if (c->worker_percentage >= 50) {
        window_building_draw_description(c, group_id, 4);
    } else if (c->worker_percentage >= 25) {
        window_building_draw_description(c, group_id, 5);
    } else {
        window_building_draw_description(c, group_id, 6);
    }
    inner_panel_draw(c->x_offset + 16, c->y_offset + 136, c->width_blocks - 2, 4);
    window_building_draw_employment(c, 142);
}

void window_building_draw_juggler_school(building_info_context* c) {
    draw_entertainment_school(c, "wavs/art_pit.wav", 77);
}

void window_building_draw_conservatory(building_info_context* c) {
    draw_entertainment_school(c, "wavs/glad_pit.wav", 75);
}

void window_building_draw_dancer_school(building_info_context* c) {
    draw_entertainment_school(c, "wavs/lion_pit.wav", 76);
}

void window_building_draw_chariot_maker(building_info_context* c) {
    draw_entertainment_school(c, "wavs/char_pit.wav", 78);
}

// TODO: fix festival square panel
void window_building_draw_festival_square(building_info_context* c) {
    const int32_t group_id = 188;
    c->help_id = 75;
    window_building_play_sound(c, "wavs/prefecture.wav"); // TODO: change to festival square

    outer_panel_draw(c->x_offset, c->y_offset, c->width_blocks, c->height_blocks);
    lang_text_draw_centered(group_id, 0, c->x_offset, c->y_offset + 10, 16 * c->width_blocks, FONT_LARGE_BLACK_ON_LIGHT);
}
