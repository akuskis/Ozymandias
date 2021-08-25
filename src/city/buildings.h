#ifndef CITY_BUILDINGS_H
#define CITY_BUILDINGS_H

#include "building/building.h"

bool city_buildings_has_senate(void);
int city_buildings_get_senate_id();
void city_buildings_add_senate(building *senate);
void city_buildings_remove_senate(building *senate);

bool city_buildings_has_mansion(void);
int city_buildings_get_mansion_id();
void city_buildings_add_mansion(building *mansion);
void city_buildings_remove_mansion(building *mansion);

bool city_buildings_has_barracks(void);
void city_buildings_add_barracks(building *barracks);
void city_buildings_remove_barracks(building *barracks);
int city_buildings_get_barracks(void);
void city_buildings_set_barracks(int building_id);

bool city_buildings_has_distribution_center(void);
void city_buildings_add_distribution_center(building *center);
void city_buildings_remove_distribution_center(building *center);

int city_buildings_get_trade_center(void);
void city_buildings_set_trade_center(int building_id);

bool city_buildings_has_hippodrome(void);
void city_buildings_add_hippodrome(void);
void city_buildings_remove_hippodrome(void);

int city_buildings_triumphal_arch_available(void);
void city_buildings_earn_triumphal_arch(void);
void city_buildings_build_triumphal_arch(void);
void city_buildings_remove_triumphal_arch(void);

void city_buildings_add_dock(void);
void city_buildings_remove_dock(void);

void city_buildings_reset_dock_wharf_counters(void);
void city_buildings_add_working_wharf(int needs_fishing_boat);
void city_buildings_add_working_dock(int building_id);
int city_buildings_shipyard_boats_requested(void);
bool city_buildings_has_working_dock(void);
int city_buildings_get_working_dock(int index);

void city_buildings_main_native_meeting_center(int *x, int *y);
void city_buildings_set_main_native_meeting_center(int x, int y);

int city_buildings_is_mission_post_operational(void);
void city_buildings_set_mission_post_operational(void);

bool city_building_has_festival_square(void);
void city_building_get_festival_square_position(int *x, int *y);
void city_buildings_add_festival_square(building *square);
void city_buildings_remove_festival_square(void);

bool city_buildings_has_temple_complex(void);
void city_buildings_add_temple_complex(building *complex);
void city_buildings_remove_temple_complex(void);

int city_buildings_unknown_value(void);

#endif // CITY_BUILDINGS_H
