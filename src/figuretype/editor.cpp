#include "editor.h"

#include "figure/image.h"
#include "graphics/image.h"
#include "grid/figure.h"
#include "grid/grid.h"
#include "scenario/editor_map.h"
#include "scenario/map.h"

void figure_create_editor_flags() {
    for (int id = MAP_FLAG_MIN; id < MAP_FLAG_MAX; id++) {
        figure* f = figure_create(FIGURE_MAP_FLAG, map_point(-1, -1), 0);
        f->set_resource((e_resource)id);
    }
}

void figure::editor_flag_action() {
    //    figure_image_increase_offset(16);
    //    f->sprite_image_id = image_id_from_group(GROUP_FIGURE_MAP_FLAG_FLAGS) + f->anim_frame / 2;
    //    map_f->map_figure_remove();
    //
    //    map_point point = {0, 0};
    //    int id = f->resource_id;
    //    int image_base = image_id_from_group(GROUP_FIGURE_MAP_FLAG_ICONS);
    //    if (id == MAP_FLAG_EARTHQUAKE) {
    //        point = scenario_editor_earthquake_point();
    //        f->cart_image_id = image_base;
    //    } else if (id == MAP_FLAG_ENTRY) {
    //        point = scenario_map_entry();
    //        f->cart_image_id = image_base + 2;
    //    } else if (id == MAP_FLAG_EXIT) {
    //        point = scenario_map_exit();
    //        f->cart_image_id = image_base + 3;
    //    } else if (id == MAP_FLAG_RIVER_ENTRY) {
    //        point = scenario_map_river_entry();
    //        f->cart_image_id = image_base + 4;
    //    } else if (id == MAP_FLAG_RIVER_EXIT) {
    //        point = scenario_map_river_exit();
    //        f->cart_image_id = image_base + 5;
    //    } else if (id >= MAP_FLAG_INVASION_MIN && id < MAP_FLAG_INVASION_MAX) {
    //        point = scenario_editor_invasion_point(id - MAP_FLAG_INVASION_MIN);
    //        f->cart_image_id = image_base + 1;
    //    } else if (id >= MAP_FLAG_FISHING_MIN && id < MAP_FLAG_FISHING_MAX) {
    //        point = scenario_editor_fishing_point(id - MAP_FLAG_FISHING_MIN);
    //        f->cart_image_id = image_id_from_group(GROUP_FIGURE_FORT_STANDARD_ICONS) + 3;
    //    } else if (id >= MAP_FLAG_HERD_MIN && id < MAP_FLAG_HERD_MAX) {
    //        point = scenario_editor_herd_point(id - MAP_FLAG_HERD_MIN);
    //        f->cart_image_id = image_id_from_group(GROUP_FIGURE_FORT_STANDARD_ICONS) + 4;
    //    }
    //    f->x = point.x;
    //    f->y = point.y;
    //
    //    f->grid_offset_figure = map_grid_offset(f->x, f->y);
    //    f->cross_country_x = 15 * f->x + 7;
    //    f->cross_country_y = 15 * f->y + 7;
    //    map_figure_add();
}
