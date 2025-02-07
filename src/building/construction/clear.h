#pragma once

/**
 * Clears land
 * @param measure_only Whether to measure only
 * @param x_start Start X
 * @param y_start Start Y
 * @param x_end End X
 * @param y_end End Y
 * @return Number of tiles cleared
 */
int building_construction_clear_land(bool measure_only, int x_start, int y_start, int x_end, int y_end);
