#include "migrant.h"

#include "building/house.h"
#include "building/model.h"
#include "city/map.h"
#include "city/population.h"
#include "core/calc.h"
#include "core/image.h"
#include "figure/combat.h"
#include "figure/image.h"
#include "figure/movement.h"
#include "figure/route.h"
#include "map/road_access.h"

void figure_create_immigrant(building *house, int num_people)
{
    const map_tile *entry = city_map_entry_point();
    figure *f = figure_create(FIGURE_IMMIGRANT, entry->x, entry->y, DIR_0_TOP);
    f->action_state = FIGURE_ACTION_1_IMMIGRANT_CREATED;
    f->immigrant_building_id = house->id;
    house->immigrant_figure_id = f->id;
    f->wait_ticks = 10 + (house->house_figure_generation_delay & 0x7f);
    f->migrant_num_people = num_people;
}
void figure_create_emigrant(building *house, int num_people)
{
    city_population_remove(num_people);
    if (num_people < house->house_population)
        house->house_population -= num_people;
    else {
        house->house_population = 0;
        building_house_change_to_vacant_lot(house);
    }
    figure *f = figure_create(FIGURE_EMIGRANT, house->x, house->y, DIR_0_TOP);
    f->action_state = FIGURE_ACTION_4_EMIGRANT_CREATED;
    f->wait_ticks = 0;
    f->migrant_num_people = num_people;
}
void figure_create_homeless(int x, int y, int num_people)
{
    figure *f = figure_create(FIGURE_HOMELESS, x, y, DIR_0_TOP);
    f->action_state = FIGURE_ACTION_7_HOMELESS_CREATED;
    f->wait_ticks = 0;
    f->migrant_num_people = num_people;
    city_population_remove_homeless(num_people);
}

void figure::update_direction_and_image()
{
//    figure_image_update(image_id_from_group(GROUP_FIGURE_MIGRANT));
    if (action_state == FIGURE_ACTION_2_IMMIGRANT_ARRIVING ||
        action_state == FIGURE_ACTION_6_EMIGRANT_LEAVING) {
        int dir = figure_image_direction();
        cart_image_id = image_id_from_group(GROUP_FIGURE_MIGRANT_CART) + dir;
        figure_image_set_cart_offset((dir + 4) % 8);
    }
}
static int closest_house_with_room(int x, int y)
{
    int min_dist = 1000;
    int min_building_id = 0;
    int max_id = building_get_highest_id();
    for (int i = 1; i <= max_id; i++) {
        building *b = building_get(i);
        if (b->state == BUILDING_STATE_IN_USE && b->house_size && b->distance_from_entry > 0 &&
            b->house_population_room > 0) {
            if (!b->immigrant_figure_id) {
                int dist = calc_maximum_distance(x, y, b->x, b->y);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_building_id = i;
                }
            }
        }
    }
    return min_building_id;

}

void figure::immigrant_action()
{
    building *b = building_get(immigrant_building_id);

//    terrain_usage = TERRAIN_USAGE_ANY;
//    cart_image_id = 0;
//    if (b->state != BUILDING_STATE_IN_USE || b->immigrant_figure_id != id || !b->house_size) {
//        state = FIGURE_STATE_DEAD;
//        return;
//    }
//
//    figure_image_increase_offset(12);

    switch (action_state) {
        case FIGURE_ACTION_1_IMMIGRANT_CREATED:
            is_ghost = 1;
            anim_frame = 0;
            wait_ticks--;
            if (wait_ticks <= 0) {
                int x_road, y_road;
                if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x_road, &y_road)) {
                    action_state = FIGURE_ACTION_2_IMMIGRANT_ARRIVING;
                    destination_x = x_road;
                    destination_y = y_road;
                    roam_length = 0;
                } else
                    state = FIGURE_STATE_DEAD;
            }
            break;
        case FIGURE_ACTION_9_HOMELESS_ENTERING_HOUSE: // pharaoh
        case FIGURE_ACTION_2_IMMIGRANT_ARRIVING:
            is_ghost = 0;
            move_ticks(1);
            switch (direction) {
                case DIR_FIGURE_AT_DESTINATION:
                    action_state = FIGURE_ACTION_3_IMMIGRANT_ENTERING_HOUSE;
                    set_cross_country_destination(b->x, b->y);
                    roam_length = 0;
                    break;
                case DIR_FIGURE_REROUTE:
                    route_remove();
                    break;
                case DIR_FIGURE_LOST:
                    b->immigrant_figure_id = 0;
                    b->distance_from_entry = 0;
                    state = FIGURE_STATE_DEAD;
                    break;
            }
            break;
        case FIGURE_ACTION_3_IMMIGRANT_ENTERING_HOUSE:
            use_cross_country = 1;
            is_ghost = 1;
            if (move_ticks_cross_country(1) == 1) {
                state = FIGURE_STATE_DEAD;
                int max_people = model_get_house(b->subtype.house_level)->max_people;
                if (b->house_is_merged)
                    max_people *= 4;

                int room = max_people - b->house_population;
                if (room < 0)
                    room = 0;

                if (room < migrant_num_people)
                    migrant_num_people = room;

                if (!b->house_population)
                    building_house_change_to(b, BUILDING_HOUSE_SMALL_TENT);

                b->house_population += migrant_num_people;
                b->house_population_room = max_people - b->house_population;
                city_population_add(migrant_num_people);
                b->immigrant_figure_id = 0;
            }
            is_ghost = in_building_wait_ticks ? 1 : 0;
            break;
    }

    update_direction_and_image();
}
void figure::emigrant_action()
{
//    terrain_usage = TERRAIN_USAGE_ANY;
//    cart_image_id = 0;
//
//    figure_image_increase_offset(12);

    switch (action_state) {
        case FIGURE_ACTION_4_EMIGRANT_CREATED:
            is_ghost = 1;
            anim_frame = 0;
            wait_ticks++;
            if (wait_ticks >= 5) {
                int x_road, y_road;
                if (!map_closest_road_within_radius(tile_x, tile_y, 1, 5, &x_road, &y_road))
                    state = FIGURE_STATE_DEAD;

                action_state = FIGURE_ACTION_5_EMIGRANT_EXITING_HOUSE;
                set_cross_country_destination(x_road, y_road);
                roam_length = 0;
            }
            break;
        case FIGURE_ACTION_5_EMIGRANT_EXITING_HOUSE:
            use_cross_country = 1;
            is_ghost = 1;
            if (move_ticks_cross_country(1) == 1) {
                const map_tile *entry = city_map_entry_point();
                action_state = FIGURE_ACTION_6_EMIGRANT_LEAVING;
                destination_x = entry->x;
                destination_y = entry->y;
                roam_length = 0;
                progress_on_tile = 15;
            }
            is_ghost = in_building_wait_ticks ? 1 : 0;
            break;
        case FIGURE_ACTION_6_EMIGRANT_LEAVING:
            use_cross_country = 0;
            is_ghost = 0;
            move_ticks(1);
            if (direction == DIR_FIGURE_AT_DESTINATION ||
                direction == DIR_FIGURE_REROUTE ||
                direction == DIR_FIGURE_LOST) {
                state = FIGURE_STATE_DEAD;
            }
            break;
    }
    update_direction_and_image();
}
void figure::homeless_action()
{
//    figure_image_increase_offset(12);
//    terrain_usage = TERRAIN_USAGE_PREFER_ROADS;

    switch (action_state) {
        case FIGURE_ACTION_7_HOMELESS_CREATED:
            anim_frame = 0;
            wait_ticks++;
            if (wait_ticks > 51) {
                int building_id = closest_house_with_room(tile_x, tile_y);
                if (building_id) {
                    building *b = building_get(building_id);
                    int x_road, y_road;
                    if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x_road, &y_road)) {
                        b->immigrant_figure_id = id;
                        immigrant_building_id = building_id;
                        action_state = FIGURE_ACTION_8_HOMELESS_GOING_TO_HOUSE;
                        destination_x = x_road;
                        destination_y = y_road;
                        roam_length = 0;
                    } else
                        state = FIGURE_STATE_DEAD;

                } else {
                    const map_tile *exit = city_map_exit_point();
                    action_state = FIGURE_ACTION_10_HOMELESS_LEAVING;
                    destination_x = exit->x;
                    destination_y = exit->y;
                    roam_length = 0;
                    wait_ticks = 0;
                }
            }
            break;
        case FIGURE_ACTION_8_HOMELESS_GOING_TO_HOUSE:
            is_ghost = 0;
            move_ticks(1);
            if (direction == DIR_FIGURE_REROUTE || direction == DIR_FIGURE_LOST) {
                building_get(immigrant_building_id)->immigrant_figure_id = 0;
                state = FIGURE_STATE_DEAD;
            } else if (direction == DIR_FIGURE_AT_DESTINATION) {
                building *b = building_get(immigrant_building_id);
                action_state = FIGURE_ACTION_9_HOMELESS_ENTERING_HOUSE;
                set_cross_country_destination(b->x, b->y);
                roam_length = 0;
            }
            break;
        case FIGURE_ACTION_9_HOMELESS_ENTERING_HOUSE:
            use_cross_country = 1;
            is_ghost = 1;
            if (move_ticks_cross_country(1) == 1) {
                state = FIGURE_STATE_DEAD;
                building *b = building_get(immigrant_building_id);
                if (immigrant_building_id && building_is_house(b->type)) {
                    int max_people = model_get_house(b->subtype.house_level)->max_people;
                    if (b->house_is_merged)
                        max_people *= 4;

                    int room = max_people - b->house_population;
                    if (room < 0)
                        room = 0;

                    if (room < migrant_num_people)
                        migrant_num_people = room;

                    if (!b->house_population)
                        building_house_change_to(b, BUILDING_HOUSE_SMALL_TENT);

                    b->house_population += migrant_num_people;
                    b->house_population_room = max_people - b->house_population;
                    city_population_add_homeless(migrant_num_people);
                    b->immigrant_figure_id = 0;
                }
            }
            break;
        case FIGURE_ACTION_10_HOMELESS_LEAVING:
            move_ticks(1);
            if (direction == DIR_FIGURE_AT_DESTINATION || direction == DIR_FIGURE_LOST)
                state = FIGURE_STATE_DEAD;
            else if (direction == DIR_FIGURE_REROUTE)
                route_remove();

            wait_ticks++;
            if (wait_ticks > 30) {
                wait_ticks = 0;
                int building_id = closest_house_with_room(tile_x, tile_y);
                if (building_id > 0) {
                    building *b = building_get(building_id);
                    int x_road, y_road;
                    if (map_closest_road_within_radius(b->x, b->y, b->size, 2, &x_road, &y_road)) {
                        b->immigrant_figure_id = id;
                        immigrant_building_id = building_id;
                        action_state = FIGURE_ACTION_8_HOMELESS_GOING_TO_HOUSE;
                        destination_x = x_road;
                        destination_y = y_road;
                        roam_length = 0;
                        route_remove();
                    }
                }
//                else {
//                    const map_tile *exit = city_map_exit_point();
//                    action_state = FIGURE_ACTION_10_HOMELESS_LEAVING;
//                    destination_x = exit->x;
//                    destination_y = exit->y;
//                    roam_length = 0;
//                    wait_ticks = 0;
//                }
            }
            break;
    }
//    figure_image_update(image_id_from_group(GROUP_FIGURE_HOMELESS));
}
