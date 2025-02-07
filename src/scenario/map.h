#ifndef SCENARIO_MAP_H
#define SCENARIO_MAP_H

#include "grid/point.h"
#include "scenario_data.h"

// void scenario_map_init(void);

const map_data_t* scenario_map_data();

int scenario_map_size(void);

void scenario_map_init_entry_exit(void);

map_point scenario_map_entry(void);

map_point scenario_map_exit(void);

int scenario_map_has_river_entry(void);

map_point scenario_map_river_entry(void);

int scenario_map_has_river_exit(void);

map_point scenario_map_river_exit(void);

void scenario_map_foreach_herd_point(void (*callback)(int x, int y));

void scenario_map_foreach_fishing_point(void (*callback)(int x, int y));

int scenario_map_closest_fishing_point(map_point tile, map_point* fish);

int scenario_map_has_flotsam(void);

#endif // SCENARIO_MAP_H
