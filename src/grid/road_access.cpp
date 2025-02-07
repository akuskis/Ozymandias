#include "road_access.h"

#include "building/building.h"
#include "building/roadblock.h"
#include "building/rotation.h"
#include "city/map.h"
#include "core/profiler.h"
#include "grid/building.h"
#include "grid/grid.h"
#include "grid/property.h"
#include "grid/road_network.h"
#include "grid/routing/routing.h"
#include "grid/routing/routing_terrain.h"
#include "grid/terrain.h"
#include "io/config/config.h"

static bool road_tile_valid_access(int grid_offset) {
    if (map_terrain_is(grid_offset, TERRAIN_ROAD)
        && (!map_terrain_is(grid_offset, TERRAIN_BUILDING) || // general case -- no buildings over road!
                                                              // exceptions: vvv
            building_at(grid_offset)->type == BUILDING_GATEHOUSE || building_at(grid_offset)->type == BUILDING_BOOTH
            || building_at(grid_offset)->type == BUILDING_BANDSTAND
            || building_at(grid_offset)->type == BUILDING_PAVILLION
            || building_at(grid_offset)->type == BUILDING_FESTIVAL_SQUARE))
        return true;
    return false;
}

void map_road_find_minimum_tile_xy(int x, int y, int sizex, int sizey, int* min_value, int* min_grid_offset) {
    int base_offset = MAP_OFFSET(x, y);
    const int *tile_delta = map_grid_adjacent_offsets_xy(sizex, sizey);
    for (; *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;

        if (!road_tile_valid_access(grid_offset))
            continue;

        int road_index = city_map_road_network_index(map_road_network_get(grid_offset));
        if (road_index < *min_value) {
            *min_value = road_index;
            *min_grid_offset = grid_offset;
        }
    }
}

bool map_has_road_access(int x, int y, int size, map_point* road) {
    return map_has_road_access_rotation(0, x, y, size, road);
}
bool burning_ruin_can_be_accessed(int x, int y, map_point* point) {
    int base_offset = MAP_OFFSET(x, y);
    for (const int* tile_delta = map_grid_adjacent_offsets(1); *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;

        if (road_tile_valid_access(grid_offset)
            || (building_at(grid_offset)->type == BUILDING_BURNING_RUIN
                && building_at(grid_offset)->fire_duration <= 0)) {
            map_point_store_result(MAP_X(grid_offset), MAP_Y(grid_offset), point);
            return true;
        }
    }
    return false;
}

bool map_has_road_access_rotation(int rotation, int x, int y, int size, map_point* road) {
    switch (rotation) {
    case 1:
        x = x - size + 1;
        break;
    case 2:
        x = x - size + 1;
        y = y - size + 1;
        break;
    case 3:
        y = y - size + 1;
        break;
    default:
        break;
    }
    int min_value = 12;
    int min_grid_offset = MAP_OFFSET(x, y);
    map_road_find_minimum_tile_xy(x, y, size, size, &min_value, &min_grid_offset);
    if (min_value < 12) {
        if (road)
            map_point_store_result(MAP_X(min_grid_offset), MAP_Y(min_grid_offset), road);
        return true;
    }
    return false;
}

// int map_has_road_access_hippodrome_rotation(int x, int y, map_point *road, int rotation) {
//     int min_value = 12;
//     int min_grid_offset = map_grid_offset(x, y);
//     int x_offset, y_offset;
//     building_rotation_get_offset_with_rotation(5, rotation, &x_offset, &y_offset);
//     find_minimum_road_tile(x, y, 5, &min_value, &min_grid_offset);
//     find_minimum_road_tile(x + x_offset, y + y_offset, 5, &min_value, &min_grid_offset);
//     building_rotation_get_offset_with_rotation(10, rotation, &x_offset, &y_offset);
//     find_minimum_road_tile(x + x_offset, y + y_offset, 5, &min_value, &min_grid_offset);
//     if (min_value < 12) {
//         if (road)
//             map_point_store_result(map_grid_offset_to_x(min_grid_offset), map_grid_offset_to_y(min_grid_offset),
//             road);
//         return 1;
//     }
//     return 0;
// }

// int map_has_road_access_hippodrome(int x, int y, map_point *road) {
//     return map_has_road_access_hippodrome_rotation(x, y, road, building_rotation_get_rotation());
// }

// TODO: fix getting road access for temple complex
bool map_has_road_access_temple_complex(int x, int y, int orientation, bool from_corner, map_point* road) {
    int sizex = 7;
    int sizey = 13;

    // correct size for orientation
    switch (orientation) {
    case 1:
    case 3:
        sizex = 13;
        sizey = 7;
        break;
    }

    // correct offset if coords are from the main building part
    if (!from_corner) {
        switch (orientation) {
        case 0:
            x -= 2;
            y -= 11;
            break;
        case 1:
            y -= 2;
            break;
        case 2:
            x -= 2;
            break;
        case 3:
            x -= 11;
            y -= 2;
            break;
        }
    }

    int min_value = 12;
    int min_grid_offset = MAP_OFFSET(x, y);
    map_road_find_minimum_tile_xy(x, y, sizex, sizey, &min_value, &min_grid_offset);
    if (min_value < 12) {
        if (road) {
            map_point_store_result(MAP_X(min_grid_offset), MAP_Y(min_grid_offset), road);
        }
        return true;
    }
    return false;
}

bool map_road_within_radius(int x, int y, int size, int radius, map_point &road_tile) {
    OZZY_PROFILER_SECTION("road_within_radius");
    int x_min, y_min, x_max, y_max;
    map_grid_get_area(tile2i(x, y), size, radius, &x_min, &y_min, &x_max, &y_max);

    for (int yy = y_min; yy <= y_max; yy++) {
        for (int xx = x_min; xx <= x_max; xx++) {
            if (map_terrain_is(MAP_OFFSET(xx, yy), TERRAIN_ROAD)) {
                // Don't spawn walkers on roadblocks
                if (building_at(xx, yy)->type == BUILDING_ROADBLOCK)
                    continue;

                road_tile.set(xx, yy);
                return true;
            }
        }
    }
    return false;
}

bool map_closest_road_within_radius(int x, int y, int size, int radius, map_point &road_tile) {
    OZZY_PROFILER_SECTION("map_closest_road_within_radius");
    for (int r = 1; r <= radius; r++) {
        if (map_road_within_radius(x, y, size, r, road_tile))
            return true;
    }
    return false;
}

bool map_reachable_road_within_radius(int x, int y, int size, int radius, map_point &road_tile) {
    OZZY_PROFILER_SECTION("reachable_road_within_radius");
    int x_min, y_min, x_max, y_max;
    map_grid_get_area(tile2i(x, y), size, radius, &x_min, &y_min, &x_max, &y_max);

    for (int yy = y_min; yy <= y_max; yy++) {
        for (int xx = x_min; xx <= x_max; xx++) {
            int grid_offset = MAP_OFFSET(xx, yy);
            if (map_terrain_is(grid_offset, TERRAIN_ROAD)) {
                if (map_routing_distance(grid_offset) > 0) {
                    road_tile = {xx, yy};
                    return true;
                }
            }
        }
    }
    return false;
}

bool map_closest_reachable_road_within_radius(int x, int y, int size, int radius, map_point &road_tile) {
    OZZY_PROFILER_SECTION("map_closest_reachable_road_within_radius");
    for (int r = 1; r <= radius; r++) {
        if (map_reachable_road_within_radius(x, y, size, r, road_tile))
            return true;
    }
    return false;
}

int map_road_to_largest_network_rotation(int rotation, int x, int y, int size, int* x_road, int* y_road) {
    switch (rotation) {
    case 1:
        x = x - size + 1;
        break;
    case 2:
        x = x - size + 1;
        y = y - size + 1;
        break;
    case 3:
        y = y - size + 1;
        break;
    default:
        break;
    }
    int min_index = 12;
    int min_grid_offset = -1;
    int base_offset = MAP_OFFSET(x, y);
    for (const int* tile_delta = map_grid_adjacent_offsets(size); *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;
        if (map_terrain_is(grid_offset, TERRAIN_ROAD) && map_routing_distance(grid_offset) > 0) {
            int index = city_map_road_network_index(map_road_network_get(grid_offset));
            if (index < min_index) {
                min_index = index;
                min_grid_offset = grid_offset;
            }
        }
    }
    if (min_index < 12) {
        *x_road = MAP_X(min_grid_offset);
        *y_road = MAP_Y(min_grid_offset);
        return min_grid_offset;
    }
    int min_dist = 100000;
    min_grid_offset = -1;
    for (const int* tile_delta = map_grid_adjacent_offsets(size); *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;
        int dist = map_routing_distance(grid_offset);
        if (dist > 0 && dist < min_dist) {
            min_dist = dist;
            min_grid_offset = grid_offset;
        }
    }
    if (min_grid_offset >= 0) {
        *x_road = MAP_X(min_grid_offset);
        *y_road = MAP_Y(min_grid_offset);
        return min_grid_offset;
    }
    return -1;
}
int map_road_to_largest_network(int x, int y, int size, int* x_road, int* y_road) {
    return map_road_to_largest_network_rotation(0, x, y, size, x_road, y_road);
}

static void check_road_to_largest_network_hippodrome(int x, int y, int* min_index, int* min_grid_offset) {
    int base_offset = MAP_OFFSET(x, y);
    for (const int* tile_delta = map_grid_adjacent_offsets(5); *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;
        if (map_terrain_is(grid_offset, TERRAIN_ROAD) && map_routing_distance(grid_offset) > 0) {
            int index = city_map_road_network_index(map_road_network_get(grid_offset));
            if (index < *min_index) {
                *min_index = index;
                *min_grid_offset = grid_offset;
            }
        }
    }
}

static void
check_min_dist_hippodrome(int base_offset, int x_offset, int* min_dist, int* min_grid_offset, int* min_x_offset) {
    for (const int* tile_delta = map_grid_adjacent_offsets(5); *tile_delta; tile_delta++) {
        int grid_offset = base_offset + *tile_delta;
        int dist = map_routing_distance(grid_offset);
        if (dist > 0 && dist < *min_dist) {
            *min_dist = dist;
            *min_grid_offset = grid_offset;
            *min_x_offset = x_offset;
        }
    }
}

int map_road_to_largest_network_hippodrome(int x, int y, int* x_road, int* y_road) {
    int min_index = 12;
    int min_grid_offset = -1;
    check_road_to_largest_network_hippodrome(x, y, &min_index, &min_grid_offset);
    check_road_to_largest_network_hippodrome(x + 5, y, &min_index, &min_grid_offset);
    check_road_to_largest_network_hippodrome(x + 10, y, &min_index, &min_grid_offset);

    if (min_index < 12) {
        *x_road = MAP_X(min_grid_offset);
        *y_road = MAP_Y(min_grid_offset);
        return min_grid_offset;
    }

    int min_dist = 100000;
    min_grid_offset = -1;
    int min_x_offset = -1;
    check_min_dist_hippodrome(MAP_OFFSET(x, y), 0, &min_dist, &min_grid_offset, &min_x_offset);
    check_min_dist_hippodrome(MAP_OFFSET(x + 5, y), 5, &min_dist, &min_grid_offset, &min_x_offset);
    check_min_dist_hippodrome(MAP_OFFSET(x + 10, y), 10, &min_dist, &min_grid_offset, &min_x_offset);

    if (min_grid_offset >= 0) {
        *x_road = MAP_X(min_grid_offset) + min_x_offset;
        *y_road = MAP_Y(min_grid_offset);
        return min_grid_offset + min_x_offset;
    }
    return -1;
}

static int terrain_is_road_like(int grid_offset) {
    return map_terrain_is(grid_offset, TERRAIN_ROAD | TERRAIN_ACCESS_RAMP) ? 1 : 0;
}

static int get_adjacent_road_tile_for_roaming(int grid_offset, int perm) {
    int is_road = terrain_is_road_like(grid_offset);
    if (map_terrain_is(grid_offset, TERRAIN_WATER) && map_terrain_is(grid_offset, TERRAIN_FLOODPLAIN))
        return 0;
    if (map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        building* b = building_at(grid_offset);
        if (b->type == BUILDING_GATEHOUSE) {
            is_road = 0;

        } else if (b->type == BUILDING_ROADBLOCK) {
            if (!building_roadblock_get_permission(perm, b)) {
                is_road = 0;
            }

        } else if (b->type == BUILDING_GRANARY) {
            if (map_routing_citizen_is_road(grid_offset)) {
                if (config_get(CONFIG_GP_CH_DYNAMIC_GRANARIES)) {
                    if (map_property_multi_tile_xy(grid_offset) == EDGE_X1Y1 || map_has_adjacent_road_tiles(grid_offset)
                        || map_has_adjacent_granary_road(grid_offset))
                        is_road = 1;

                } else {
                    is_road = 1;
                }
            }
        } else if (b->type == BUILDING_TRIUMPHAL_ARCH) {
            if (map_routing_citizen_is_road(grid_offset))
                is_road = 1;
        }
    }
    return is_road;
}

int map_get_adjacent_road_tiles_for_roaming(int grid_offset, int* road_tiles, int perm) {
    road_tiles[1] = road_tiles[3] = road_tiles[5] = road_tiles[7] = 0;

    road_tiles[0] = get_adjacent_road_tile_for_roaming(grid_offset + GRID_OFFSET(0, -1), perm);
    road_tiles[2] = get_adjacent_road_tile_for_roaming(grid_offset + GRID_OFFSET(1, 0), perm);
    road_tiles[4] = get_adjacent_road_tile_for_roaming(grid_offset + GRID_OFFSET(0, 1), perm);
    road_tiles[6] = get_adjacent_road_tile_for_roaming(grid_offset + GRID_OFFSET(-1, 0), perm);

    return road_tiles[0] + road_tiles[2] + road_tiles[4] + road_tiles[6];
}

int map_get_diagonal_road_tiles_for_roaming(int grid_offset, int* road_tiles) {
    road_tiles[1] = terrain_is_road_like(grid_offset + GRID_OFFSET(1, -1));
    road_tiles[3] = terrain_is_road_like(grid_offset + GRID_OFFSET(1, 1));
    road_tiles[5] = terrain_is_road_like(grid_offset + GRID_OFFSET(-1, 1));
    road_tiles[7] = terrain_is_road_like(grid_offset + GRID_OFFSET(-1, -1));

    int max_stretch = 0;
    int stretch = 0;
    for (int i = 0; i < 16; i++) {
        if (road_tiles[i % 8]) {
            stretch++;
            if (stretch > max_stretch)
                max_stretch = stretch;

        } else {
            stretch = 0;
        }
    }
    return max_stretch;
}

int map_has_adjacent_road_tiles(int grid_offset) {
    int adjacent_roads = terrain_is_road_like(grid_offset + GRID_OFFSET(0, -1))
                         + terrain_is_road_like(grid_offset + GRID_OFFSET(1, 0))
                         + terrain_is_road_like(grid_offset + GRID_OFFSET(0, 1))
                         + terrain_is_road_like(grid_offset + GRID_OFFSET(-1, 0));
    return adjacent_roads;
}

int map_has_adjacent_granary_road(int grid_offset) {
    int tiles[4];
    tiles[0] = grid_offset + GRID_OFFSET(0, -1);
    tiles[1] = grid_offset + GRID_OFFSET(1, 0);
    tiles[2] = grid_offset + GRID_OFFSET(0, 1);
    tiles[3] = grid_offset + GRID_OFFSET(-1, 0);
    for (int i = 0; i < 4; i++) {
        if (building_at(tiles[i])->type != BUILDING_GRANARY)
            continue;
        switch (map_property_multi_tile_xy(tiles[i])) {
        case EDGE_X1Y0:
        case EDGE_X0Y1:
        case EDGE_X2Y1:
        case EDGE_X1Y2:
            return 1;
        }
    }
    return 0;
}
