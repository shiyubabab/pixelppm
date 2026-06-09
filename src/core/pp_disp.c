/*************************************************************************
	> File Name: pp_disp.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sun Jun  7 20:22:00 2026
	> Description: Clean display entity constructor and state management tracker.
 ************************************************************************/

#include "core/pp_disp.h"
#include "helper/pp_mem.h"
#include <string.h>

pp_disp_t * global_display = NULL;

bool pp_disp_init(int32_t hor_res, int32_t ver_res, pp_render_mode_t mode, uint32_t max_rows)
{
    if (hor_res <= 0 || ver_res <= 0) return false;

    global_display = (pp_disp_t *)pp_malloc(sizeof(pp_disp_t));
    if (!global_display) return false;

    global_display->hor_res = hor_res;
    global_display->ver_res = ver_res;
    global_display->inv_p = 0;
    global_display->render_mode = mode;
    global_display->max_partial_rows = (max_rows == 0 || max_rows > (uint32_t)ver_res) ? (uint32_t)ver_res : max_rows;

    // Spawn canvas payload
    global_display->canvas = pp_canvas_create(hor_res, ver_res);
    if (!global_display->canvas) {
        pp_free(global_display);
        return false;
    }

    // Spawn layout root object
    global_display->root_obj = pp_obj_create(NULL);
    if (!global_display->root_obj) {
        pp_canvas_destroy(global_display->canvas);
        pp_free(global_display);
        return false;
    }

    // Force base node boundaries to stretch safely across full screen resolution bounds
    global_display->root_obj->parent = NULL;
    global_display->root_obj->coords.x1 = 0;
    global_display->root_obj->coords.y1 = 0;
    global_display->root_obj->coords.x2 = hor_res - 1;
    global_display->root_obj->coords.y2 = ver_res - 1;
    global_display->root_obj->user_data = NULL;

    memset(global_display->inv_areas, 0, sizeof(global_display->inv_areas));
    memset(global_display->inv_area_joined, 0, sizeof(global_display->inv_area_joined));

    PP_DISP_INFO("Display core device successfully linked up. [%dx%d]", hor_res, ver_res);
    return true;
}

void pp_disp_deinit(void)
{
    if (!global_display) return;
    if (global_display->canvas) pp_canvas_destroy(global_display->canvas);
    if (global_display->root_obj) pp_free(global_display->root_obj);
    pp_free(global_display);
    global_display = NULL;
    PP_DISP_INFO("Display device context destroyed successfully.");
}

void pp_disp_invalidate_area(const pp_area_t * area_p)
{
    if (!global_display || !area_p) return;

    pp_area_t screen_box = { 0, 0, global_display->hor_res - 1, global_display->ver_res - 1 };
    pp_area_t cropped_dirty_zone;

    // Clamp bounds using our established pp_area_intersect to secure 0 overdraw waste
    if (!pp_area_intersect(&cropped_dirty_zone, area_p, &screen_box)) {
        return; 
    }

    // Safeguard lookup ledger limitations
    if (global_display->inv_p >= 32) {
        global_display->inv_areas[0] = screen_box;
        global_display->inv_area_joined[0] = 0;
        global_display->inv_p = 1;
        return;
    }

    global_display->inv_areas[global_display->inv_p] = cropped_dirty_zone;
    global_display->inv_area_joined[global_display->inv_p] = 0;
    global_display->inv_p++;
}
