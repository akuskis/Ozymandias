#pragma once

#include "tile_cache.h"
#include <stdint.h>

#define MAX_FLOODPLAIN_ROWS 30

enum e_fert_type { 
    FERT_NO_MALUS = 0,
    FERT_WITH_MALUS = 1,
    FERT_ONLY_MALUS = 2
};

extern tile_cache floodplain_tiles_cache;

void foreach_floodplain_row(int row, void (*callback)(int grid_offset, int order));
int map_floodplain_rebuild_rows();
int8_t map_get_floodplain_row(int grid_offset);

uint8_t map_get_floodplain_growth(int grid_offset);
void map_clear_floodplain_growth();
void map_tiles_update_floodplain_images();
int map_get_fertility(int grid_offset, int tally_type);
uint8_t map_get_fertility_for_farm(int grid_offset);
void map_set_floodplain_growth(int grid_offset, int growth);
void map_soil_set_depletion(int grid_offset, int malus);
void map_update_tile_fertility(int grid_offset, int delta);
void map_update_area_fertility(int grid_offset, int x, int y, int size, int delta);

int map_get_UNK04(int grid_offset);
