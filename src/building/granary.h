#pragma once

#include "game/resource.h"

class building;
class map_point;

enum e_granary_task {
    GRANARY_TASK_NONE = -1,
    GRANARY_TASK_GETTING = 0,
};

struct granary_task_status {
    e_granary_task status;
    e_resource resource;
};

int building_granary_get_amount(building* granary, int resource);

int building_granary_add_resource(building* granary, int resource, int is_produced, int amount);

int building_granary_remove_resource(building* granary, int resource, int amount);

int building_granary_remove_for_getting_deliveryman(building* src, building* dst, e_resource& resource);

int building_granary_is_not_accepting(int resource, building* b);

granary_task_status building_granary_determine_worker_task(building* granary);

void building_granaries_calculate_stocks(void);

int building_granary_for_storing(map_point tile,
                                 int resource,
                                 int distance_from_entry,
                                 int road_network_id,
                                 int force_on_stockpile,
                                 int* understaffed,
                                 map_point* dst);

int building_getting_granary_for_storing(map_point tile,
                                         int resource,
                                         int distance_from_entry,
                                         int road_network_id,
                                         map_point* dst);

int building_granary_for_getting(building* src, map_point* dst);

void building_granary_bless(void);
void building_granary_warehouse_curse(int big);

bool building_granary_is_getting(int resource, building* b);
