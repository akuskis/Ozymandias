#pragma once

void empire_load_editor(int empire_id, int viewport_width, int viewport_height);

void empire_init_scenario(void);
void empire_scroll_map(int x, int y);
void empire_set_viewport(int width, int height);
void empire_get_scroll(int* x_scroll, int* y_scroll);
void empire_set_scroll(int x, int y);
void empire_adjust_scroll(int* x_offset, int* y_offset);

int empire_selected_object(void);

void empire_clear_selected_object(void);
void empire_select_object(int x, int y);

bool empire_can_export_resource_to_city(int city_id, int resource);
int empire_can_import_resource_from_city(int city_id, int resource);
