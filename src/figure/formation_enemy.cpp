#include "formation_enemy.h"

#include "building/building.h"
#include "city/buildings.h"
#include "city/figures.h"
#include "city/gods.h"
#include "city/message.h"
#include "core/calc.h"
#include "core/random.h"
#include "figure/enemy_army.h"
#include "figure/figure.h"
#include "figure/formation.h"
#include "figure/formation_layout.h"
#include "figure/route.h"
#include "grid/figure.h"
#include "grid/grid.h"
#include "grid/routing/queue.h"
#include "grid/routing/routing.h"
#include "grid/soldier_strength.h"
#include "grid/terrain.h"

static const int ENEMY_ATTACK_PRIORITY[4][100] = {
  {BUILDING_GRANARY,
   BUILDING_STORAGE_YARD,
   BUILDING_MARKET,
   BUILDING_BARLEY_FARM,
   BUILDING_FLAX_FARM,
   BUILDING_GRAIN_FARM,
   BUILDING_LETTUCE_FARM,
   BUILDING_POMEGRANATES_FARM,
   BUILDING_CHICKPEAS_FARM,
   0},

  {BUILDING_GREATE_PALACE_UPGRADED, BUILDING_GREATE_PALACE, BUILDING_TAX_COLLECTOR_UPGRADED, BUILDING_TAX_COLLECTOR, 0},

  {BUILDING_DYNASTY_MANSION,    BUILDING_FAMILY_MANSION,      BUILDING_PERSONAL_MANSION,   BUILDING_HOUSE_LUXURY_PALACE,
   BUILDING_HOUSE_LARGE_PALACE, BUILDING_HOUSE_MEDIUM_PALACE, BUILDING_HOUSE_SMALL_PALACE, BUILDING_HOUSE_GRAND_VILLA,
   BUILDING_HOUSE_LARGE_VILLA,  BUILDING_HOUSE_MEDIUM_VILLA,  BUILDING_HOUSE_SMALL_VILLA,  BUILDING_HOUSE_GRAND_INSULA,
   BUILDING_HOUSE_LARGE_INSULA, BUILDING_HOUSE_MEDIUM_INSULA, BUILDING_HOUSE_SMALL_INSULA, BUILDING_HOUSE_LARGE_CASA,
   BUILDING_HOUSE_SMALL_CASA,   BUILDING_HOUSE_LARGE_HOVEL,   BUILDING_HOUSE_SMALL_HOVEL,  BUILDING_HOUSE_LARGE_SHACK,
   BUILDING_HOUSE_SMALL_SHACK,  BUILDING_HOUSE_LARGE_HUT,    BUILDING_HOUSE_SMALL_HUT,   0},
  {BUILDING_MILITARY_ACADEMY, BUILDING_POLICE_STATION, 0}};

static const int RIOTER_ATTACK_PRIORITY[100] = {
    79,  78,  77,  29, 28,  27,  26,  25,  85,  84,  32, 33, 98, 65, 66,  67,  68,  69,  87,  86, 30,
    31,  47,  52,  46, 48,  53,  51,  24,  23,  22,  21, 20, 46, 48, 114, 113, 112, 111, 110, 71, 72,
    70,  74,  75,  76, 60,  61,  62,  63,  64,  34,  36, 37, 35, 94, 19,  18,  17,  16,  15,  49, 106,
    107, 109, 108, 90, 100, 101, 102, 103, 104, 105, 55, 81, 91, 92, 14,  13,  12,  11,  10,  0
};

static const int LAYOUT_ORIENTATION_OFFSETS[13][4][40]= {
      {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -6, 0, 6, 0, -6, 2, 6, 2, -2, 4, 4, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, 2, -6, 2, 6, 4, -2, 6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -6, 0, 6, 0, -6, -2, 6, -2, -4, -6, 4, -6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, -2, -6, -2, 6, -6, -4, -6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -6, 0, 6, 0, -6, 2, 6, 2, -2, 4, 4, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, 2, -6, 2, 6, 4, -2, 6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -6, 0, 6, 0, -6, -2, 6, -2, -4, -6, 4, -6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, -2, -6, -2, 6, -6, -4, -6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -6, 0, 6, 0, -6, 2, 6, 2, -2, 4, 4, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, 2, -6, 2, 6, 4, -2, 6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -6, 0, 6, 0, -6, -2, 6, -2, -4, -6, 4, -6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, -2, -6, -2, 6, -6, -4, -6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -6, 0, 6, 0, -6, 2, 6, 2, -2, 4, 4, 6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, 2, -6, 2, 6, 4, -2, 6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -6, 0, 6, 0, -6, -2, 6, -2, -4, -6, 4, -6, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -6, 0, 6, -2, -6, -2, 6, -6, -4, -6, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -4, 0, 4, 0, -12, 0, 12, 0, -4, 12, 4, 12, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,   0, 0,  0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -4, 0, 4, 0, -12, 0, 12, 12, -4, 12, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
       {0, 0, -4, 0, 4, 0, -12, 0, 12, 0, -4, -12, 4, -12, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,   0, 0,  0, 0,  0,   0, 0,   0, 0, 0, 0, 0, 0},
       {0, 0, 0, -4, 0, 4, 0, -12, 0, 12, -12, -4, -12, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0,  0,   0,  0,   0, 0, 0, 0, 0, 0, 0},
     },
     {
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, 8, 3, 8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, 8, -3, 8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
       {0, 0, -3, 0, 3, 0, -8, 0, 8, 0, -3, -8, 3, -8, 0, 0, 0, 0, 0, 0,
        0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0, 0, 0, 0, 0},
       {0, 0, 0, -3, 0, 3, 0, -8, 0, 8, -8, -3, -8, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0},
     }
};

int formation_rioter_get_target_building(int* x_tile, int* y_tile) {
    int best_type_index = 100;
    building* best_building = 0;
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_VALID)
            continue;

        for (int t = 0; t < 100 && t <= best_type_index && RIOTER_ATTACK_PRIORITY[t]; t++) {
            if (b->type == RIOTER_ATTACK_PRIORITY[t]) {
                if (t < best_type_index) {
                    best_type_index = t;
                    best_building = b;
                }
                break;
            }
        }
    }
    if (!best_building)
        return 0;

    if (best_building->type == BUILDING_STORAGE_YARD) {
        *x_tile = best_building->tile.x() + 1;
        *y_tile = best_building->tile.y();
        return best_building->id + 1;
    } else {
        *x_tile = best_building->tile.x();
        *y_tile = best_building->tile.y();
        return best_building->id;
    }
}

static void set_enemy_target_building(formation* m) {
    int attack = m->attack_type;
    if (attack == FORMATION_ATTACK_RANDOM)
        attack = random_byte() & 3;

    int best_type_index = 100;
    building* best_building = 0;
    int min_distance = 10000;
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_VALID || map_soldier_strength_get(b->tile.grid_offset()))
            continue;

        for (int n = 0; n < 100 && n <= best_type_index && ENEMY_ATTACK_PRIORITY[attack][n]; n++) {
            if (b->type == ENEMY_ATTACK_PRIORITY[attack][n]) {
                int distance = calc_maximum_distance(vec2i(m->x_home, m->y_home), b->tile);
                if (n < best_type_index) {
                    best_type_index = n;
                    best_building = b;
                    min_distance = distance;
                } else if (distance < min_distance) {
                    best_building = b;
                    min_distance = distance;
                }
                break;
            }
        }
    }
    if (!best_building) {
        // no target buildings left: take rioter attack priority
        for (int i = 1; i < MAX_BUILDINGS; i++) {
            building* b = building_get(i);
            if (b->state != BUILDING_STATE_VALID || map_soldier_strength_get(b->tile.grid_offset()))
                continue;

            for (int n = 0; n < 100 && n <= best_type_index && RIOTER_ATTACK_PRIORITY[n]; n++) {
                if (b->type == RIOTER_ATTACK_PRIORITY[n]) {
                    int distance = calc_maximum_distance(vec2i(m->x_home, m->y_home), b->tile);
                    if (n < best_type_index) {
                        best_type_index = n;
                        best_building = b;
                        min_distance = distance;
                    } else if (distance < min_distance) {
                        best_building = b;
                        min_distance = distance;
                    }
                    break;
                }
            }
        }
    }
    if (best_building) {
        if (best_building->type == BUILDING_STORAGE_YARD)
            formation_set_destination_building(m,
                                               best_building->tile.x() + 1,
                                               best_building->tile.y(),
                                               best_building->id + 1);
        else {
            formation_set_destination_building(m, best_building->tile.x(), best_building->tile.y(), best_building->id);
        }
    }
}

static void set_native_target_building(formation* m) {
    map_point meeting = city_buildings_main_native_meeting_center();
    building* min_building = 0;
    int min_distance = 10000;
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_VALID)
            continue;

        switch (b->type) {
        case BUILDING_MISSION_POST:
        case BUILDING_NATIVE_HUT:
        case BUILDING_NATIVE_CROPS:
        case BUILDING_NATIVE_MEETING:
        case BUILDING_STORAGE_YARD:
        case BUILDING_MENU_FORTS:
        case BUILDING_ROADBLOCK:
            break;
        default: {
            int distance = calc_maximum_distance(meeting, b->tile);
            if (distance < min_distance) {
                min_building = b;
                min_distance = distance;
            }
        }
        }
    }

    if (min_building) {
        formation_set_destination_building(m, min_building->tile.x(), min_building->tile.y(), min_building->id);
    }
}

static void approach_target(formation* m) {
    if (map_routing_noncitizen_can_travel_over_land(
          m->x_home, m->y_home, m->destination_x, m->destination_y, m->destination_building_id, 400)
        || map_routing_noncitizen_can_travel_through_everything(
          m->x_home, m->y_home, m->destination_x, m->destination_y)) {
        int x_tile, y_tile;
        if (map_routing_get_closest_tile_within_range(
              m->x_home, m->y_home, m->destination_x, m->destination_y, 8, 20, &x_tile, &y_tile))
            formation_set_destination(m, x_tile, y_tile);
    }
}

static void set_figures_to_initial(const formation* m) {
    for (int i = 0; i < MAX_FORMATION_FIGURES; i++) {
        if (m->figures[i] > 0) {
            figure* f = figure_get(m->figures[i]);
            if (f->action_state != FIGURE_ACTION_149_CORPSE && f->action_state != FIGURE_ACTION_150_ATTACK) {
                f->action_state = FIGURE_ACTION_151_ENEMY_INITIAL;
                f->wait_ticks = 0;
            }
        }
    }
}

int formation_enemy_move_formation_to(const formation* m, int x, int y, int* x_tile, int* y_tile) {
    int base_offset = MAP_OFFSET(formation_layout_position_x(m->layout, 0), formation_layout_position_y(m->layout, 0));
    int figure_offsets[50];
    figure_offsets[0] = 0;
    for (int i = 1; i < m->num_figures; i++) {
        figure_offsets[i] = MAP_OFFSET(formation_layout_position_x(m->layout, i), formation_layout_position_y(m->layout, i)) - base_offset;
    }
    map_routing_noncitizen_can_travel_over_land(x, y, -1, -1, 0, 600);
    for (int r = 0; r <= 10; r++) {
        int x_min, y_min, x_max, y_max;
        map_grid_get_area(tile2i(x, y), 1, r, &x_min, &y_min, &x_max, &y_max);
        for (int yy = y_min; yy <= y_max; yy++) {
            for (int xx = x_min; xx <= x_max; xx++) {
                int can_move = 1;
                for (int fig = 0; fig < m->num_figures; fig++) {
                    int grid_offset = MAP_OFFSET(xx, yy) + figure_offsets[fig];
                    if (!map_grid_is_valid_offset(grid_offset)) {
                        can_move = 0;
                        break;
                    }

                    if (map_terrain_is(grid_offset, TERRAIN_IMPASSABLE_ENEMY)) {
                        can_move = 0;
                        break;
                    }

                    if (map_routing_distance(grid_offset) <= 0) {
                        can_move = 0;
                        break;
                    }

                    if (map_has_figure_at(grid_offset) && figure_get(map_figure_id_get(grid_offset))->formation_id != m->id) {
                        can_move = 0;
                        break;
                    }
                }
                if (can_move) {
                    *x_tile = xx;
                    *y_tile = yy;
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void seth_kill_enemies(void) {
    int to_kill = city_god_spirit_of_seth_power();
    if (to_kill <= 0)
        return;
    int grid_offset = 0;
    for (int i = 1; i < MAX_FIGURES[GAME_ENV] && to_kill > 0; i++) {
        figure* f = figure_get(i);
        if (f->state != FIGURE_STATE_ALIVE)
            continue;

        if (f->is_enemy() && f->type != FIGURE_ENEMY54_GLADIATOR) {
            f->kill();
            to_kill--;
            if (!grid_offset)
                grid_offset = f->tile.grid_offset();
        }
    }
    city_god_spirit_of_seth_mark_used();
    city_message_post(true, MESSAGE_SPIRIT_OF_MARS, 0, grid_offset);
}

static void update_enemy_movement(formation* m, int roman_distance) {
    const enemy_army* army = enemy_army_get(m->invasion_id);
    formation_state* state = &m->enemy_state;
    int regroup = 0;
    int halt = 0;
    int pursue_target = 0;
    int advance = 0;
    int target_formation_id = 0;
    if (m->missile_fired)
        halt = 1;
    else if (m->missile_attack_timeout) {
        pursue_target = 1;
        target_formation_id = m->missile_attack_formation_id;
    } else if (m->wait_ticks < 32) {
        regroup = 1;
        state->duration_advance = 4;
    } else if (army->ignore_roman_soldiers) {
        halt = 0;
        regroup = 0;
        advance = 1;
    } else {
        int halt_duration, advance_duration, regroup_duration;
        if (army->layout == FORMATION_ENEMY_MOB || army->layout == FORMATION_ENEMY12) {
            switch (m->enemy_legion_index) {
            case 0:
            case 1:
                regroup_duration = 2;
                advance_duration = 4;
                halt_duration = 2;
                break;
            case 2:
            case 3:
                regroup_duration = 2;
                advance_duration = 5;
                halt_duration = 3;
                break;
            default:
                regroup_duration = 2;
                advance_duration = 6;
                halt_duration = 4;
                break;
            }
            if (!roman_distance) {
                advance_duration += 6;
                halt_duration--;
                regroup_duration--;
            }
        } else {
            if (roman_distance) {
                regroup_duration = 6;
                advance_duration = 4;
                halt_duration = 2;
            } else {
                regroup_duration = 1;
                advance_duration = 12;
                halt_duration = 1;
            }
        }
        if (state->duration_halt) {
            state->duration_advance = 0;
            state->duration_regroup = 0;
            halt = 1;
            state->duration_halt--;
            if (state->duration_halt <= 0) {
                state->duration_regroup = regroup_duration;
                set_figures_to_initial(m);
                regroup = 0;
                halt = 1;
            }
        } else if (state->duration_regroup) {
            state->duration_advance = 0;
            state->duration_halt = 0;
            regroup = 1;
            state->duration_regroup--;
            if (state->duration_regroup <= 0) {
                state->duration_advance = advance_duration;
                set_figures_to_initial(m);
                advance = 1;
                regroup = 0;
            }
        } else {
            state->duration_regroup = 0;
            state->duration_halt = 0;
            advance = 1;
            state->duration_advance--;
            if (state->duration_advance <= 0) {
                state->duration_halt = halt_duration;
                set_figures_to_initial(m);
                halt = 1;
                advance = 0;
            }
        }
    }

    if (m->wait_ticks > 32) {
        seth_kill_enemies();
    }

    if (halt) {
        formation_set_destination(m, m->x_home, m->y_home);
    } else if (pursue_target) {
        if (target_formation_id > 0) {
            const formation* target = formation_get(target_formation_id);
            if (target->num_figures > 0)
                formation_set_destination(m, target->x_home, target->y_home);

        } else {
            formation_set_destination(m, army->destination_x, army->destination_y);
        }
    } else if (regroup) {
        int layout = army->layout;
        int x_offset = LAYOUT_ORIENTATION_OFFSETS[layout][m->orientation / 2][2 * m->enemy_legion_index] + army->home_x;
        int y_offset
          = LAYOUT_ORIENTATION_OFFSETS[layout][m->orientation / 2][2 * m->enemy_legion_index + 1] + army->home_y;
        int x_tile, y_tile;
        if (formation_enemy_move_formation_to(m, x_offset, y_offset, &x_tile, &y_tile))
            formation_set_destination(m, x_tile, y_tile);

    } else if (advance) {
        int layout = army->layout;
        int x_offset
          = LAYOUT_ORIENTATION_OFFSETS[layout][m->orientation / 2][2 * m->enemy_legion_index] + army->destination_x;
        int y_offset
          = LAYOUT_ORIENTATION_OFFSETS[layout][m->orientation / 2][2 * m->enemy_legion_index + 1] + army->destination_y;
        int x_tile, y_tile;
        if (formation_enemy_move_formation_to(m, x_offset, y_offset, &x_tile, &y_tile))
            formation_set_destination(m, x_tile, y_tile);
    }
}

static void update_enemy_formation(formation* m, int* roman_distance) {
    enemy_army* army = enemy_army_get_editable(m->invasion_id);
    if (enemy_army_is_stronger_than_legions()) {
        if (m->figure_type != FIGURE_FORT_JAVELIN)
            army->ignore_roman_soldiers = 1;
    }
    formation_decrease_monthly_counters(m);
    if (city_figures_soldiers() <= 0)
        formation_clear_monthly_counters(m);

    for (int n = 0; n < MAX_FORMATION_FIGURES; n++) {
        figure* f = figure_get(m->figures[n]);
        if (f->action_state == FIGURE_ACTION_150_ATTACK) {
            figure* opponent = figure_get(f->opponent_id);
            if (!opponent->is_dead() && opponent->is_legion())
                formation_record_fight(m);
        }
    }
    if (formation_has_low_morale(m)) {
        for (int n = 0; n < MAX_FORMATION_FIGURES; n++) {
            figure* f = figure_get(m->figures[n]);
            if (f->action_state != FIGURE_ACTION_150_ATTACK && f->action_state != FIGURE_ACTION_149_CORPSE
                && f->action_state != FIGURE_ACTION_148_FLEEING) {
                f->action_state = FIGURE_ACTION_148_FLEEING;
                f->route_remove();
            }
        }
        return;
    }
    if (m->figures[0]) {
        figure* f = figure_get(m->figures[0]);
        if (f->state == FIGURE_STATE_ALIVE)
            formation_set_home(m, f->tile.x(), f->tile.y());
    }
    if (!army->formation_id) {
        army->formation_id = m->id;
        army->home_x = m->x_home;
        army->home_y = m->y_home;
        army->layout = m->layout;
        *roman_distance = 0;
        map_routing_noncitizen_can_travel_over_land(m->x_home, m->y_home, -2, -2, 100000, 300);
        int x_tile, y_tile;
        if (map_soldier_strength_get_max(m->x_home, m->y_home, 16, &x_tile, &y_tile))
            *roman_distance = 1;
        else if (map_soldier_strength_get_max(m->x_home, m->y_home, 32, &x_tile, &y_tile))
            *roman_distance = 2;

        if (army->ignore_roman_soldiers)
            *roman_distance = 0;

        if (*roman_distance == 1) {
            // attack roman legion
            army->destination_x = x_tile;
            army->destination_y = y_tile;
            army->destination_building_id = 0;
        } else {
            set_enemy_target_building(m);
            approach_target(m);
            army->destination_x = m->destination_x;
            army->destination_y = m->destination_y;
            army->destination_building_id = m->destination_building_id;
        }
    }
    m->enemy_legion_index = army->num_legions++;
    m->wait_ticks++;
    formation_set_destination_building(m, army->destination_x, army->destination_y, army->destination_building_id);

    update_enemy_movement(m, *roman_distance);
}

void formation_enemy_update(void) {
    if (enemy_army_total_enemy_formations() <= 0)
        enemy_armies_clear_ignore_roman_soldiers();
    else {
        enemy_army_calculate_roman_influence();
        enemy_armies_clear_formations();
        int roman_distance = 0;
        for (int i = 1; i < MAX_FORMATIONS; i++) {
            formation* m = formation_get(i);
            if (m->in_use && !m->is_herd && !m->is_legion)
                update_enemy_formation(m, &roman_distance);
        }
    }
    set_native_target_building(formation_get(0));
}
