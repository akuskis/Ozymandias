#include "service.h"

#include "building/building.h"
#include "building/market.h"
#include "figure/combat.h"
#include "figure/image.h"
#include "figure/movement.h"
#include "figure/route.h"
#include "core/profiler.h"
#include "graphics/image.h"
#include "graphics/image_groups.h"
#include "grid/building.h"
#include "grid/road_access.h"
#include "io/config/config.h"

void figure::school_child_action() {
    switch (action_state) {
    case ACTION_10_DELIVERING_FOOD:
    case FIGURE_ACTION_125_ROAMING:
        //            is_ghost = false;
        roam_length++;
        if (roam_length >= max_roam_length)
            poof();

        roam_ticks(2);
        break;
    }
}
void figure::priest_action() {
    if (GAME_ENV == ENGINE_ENV_C3)
        return;
    building* temple = home();
    switch (temple->type) {
    case BUILDING_TEMPLE_OSIRIS:
    case BUILDING_TEMPLE_COMPLEX_OSIRIS:
        image_set_animation(GROUP_PRIEST_OSIRIS);
        break;

    case BUILDING_TEMPLE_RA:
    case BUILDING_TEMPLE_COMPLEX_RA:
        image_set_animation(GROUP_PRIEST_RA);
        break;

    case BUILDING_TEMPLE_PTAH:
    case BUILDING_TEMPLE_COMPLEX_PTAH:
        image_set_animation(GROUP_PRIEST_PTAH);
        break;

    case BUILDING_TEMPLE_SETH:
    case BUILDING_TEMPLE_COMPLEX_SETH:
        image_set_animation(GROUP_PRIEST_SETH);
        break;

    case BUILDING_TEMPLE_BAST:
    case BUILDING_TEMPLE_COMPLEX_BAST:
        image_set_animation(GROUP_PRIEST_BAST);
        break;
    }
}

void figure::market_trader_action() {
    building* market = home();
    if (action_state == FIGURE_ACTION_125_ROAMING) {
        // force return on out of stock
        int stock = building_market_get_max_food_stock(market) + building_market_get_max_goods_stock(market);
        if (roam_length >= 96 && stock <= 0)
            roam_length = max_roam_length;
    }
}