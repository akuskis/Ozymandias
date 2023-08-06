#include "house_service.h"

#include "building/building.h"
#include "building/type.h"
#include "city/culture.h"
#include "core/game_environment.h"

static void decay(unsigned char* value) {
    if (*value > 0)
        *value = *value - 1;
    else {
        *value = 0;
    }
}

void house_service_decay_culture(void) {
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_VALID || !b->house_size)
            continue;

        decay(&b->data.house.theater);
        decay(&b->data.house.amphitheater_actor);
        decay(&b->data.house.amphitheater_gladiator);
        decay(&b->data.house.colosseum_gladiator);
        decay(&b->data.house.magistrate);
        decay(&b->data.house.hippodrome);
        decay(&b->data.house.school);
        decay(&b->data.house.library);
        decay(&b->data.house.academy);
        decay(&b->data.house.barber);
        decay(&b->data.house.clinic);
        decay(&b->data.house.bathhouse);
        decay(&b->data.house.hospital);
        decay(&b->data.house.temple_osiris);
        decay(&b->data.house.temple_ra);
        decay(&b->data.house.temple_ptah);
        decay(&b->data.house.temple_seth);
        decay(&b->data.house.temple_bast);
    }
}

void house_service_decay_tax_collector(void) {
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state == BUILDING_STATE_VALID && b->house_tax_coverage)
            b->house_tax_coverage--;
    }
}

#include "building/industry.h"

void house_service_decay_houses_covered(void) {
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_UNUSED) { // b->type != BUILDING_TOWER
            if (b->houses_covered > 0)
                b->houses_covered--;
            //            if (building_is_farm(b->type) && b->data.industry.labor_days_left > 0)
            //                b->data.industry.labor_days_left--;
            ////            else if (b->houses_covered > 0)
            ////                b->houses_covered--;
            //
            ////            if (b->houses_covered == 255)
            ////                b->houses_covered = 0;
            //            if (GAME_ENV == ENGINE_ENV_PHARAOH) {
            //                if (b->data.industry.labor_state == 2)
            //                    b->data.industry.labor_state = 1;
            //                if (b->data.industry.labor_days_left == 0)
            //                    b->data.industry.labor_state = 0;
            //            }
        }
    }
}

void house_service_calculate_culture_aggregates(void) {
    int base_entertainment = city_culture_coverage_average_entertainment() / 5;
    for (int i = 1; i < MAX_BUILDINGS; i++) {
        building* b = building_get(i);
        if (b->state != BUILDING_STATE_VALID || !b->house_size)
            continue;

        // entertainment
        b->data.house.entertainment = base_entertainment;
        if (b->data.house.theater)
            b->data.house.entertainment += 10;

        if (b->data.house.amphitheater_actor) {
            if (b->data.house.amphitheater_gladiator)
                b->data.house.entertainment += 15;
            else {
                b->data.house.entertainment += 10;
            }
        }
        if (b->data.house.colosseum_gladiator) {
            //            if (b->data.house.magistrate)
            //                b->data.house.entertainment += 25;
            //            else {
            b->data.house.entertainment += 15;
            //            }
        }
        if (b->data.house.hippodrome)
            b->data.house.entertainment += 30;

        // education
        b->data.house.education = 0;
        if (b->data.house.school || b->data.house.library) {
            b->data.house.education = 1;
            if (b->data.house.school && b->data.house.library) {
                b->data.house.education = 2;
                if (b->data.house.academy)
                    b->data.house.education = 3;
            }
        }

        // religion
        b->data.house.num_gods = 0;
        if (b->data.house.temple_osiris)
            ++b->data.house.num_gods;

        if (b->data.house.temple_ra)
            ++b->data.house.num_gods;

        if (b->data.house.temple_ptah)
            ++b->data.house.num_gods;

        if (b->data.house.temple_seth)
            ++b->data.house.num_gods;

        if (b->data.house.temple_bast)
            ++b->data.house.num_gods;

        // health
        b->data.house.health = 0;
        if (b->data.house.clinic)
            ++b->data.house.health;

        if (b->data.house.hospital)
            ++b->data.house.health;
    }
}
