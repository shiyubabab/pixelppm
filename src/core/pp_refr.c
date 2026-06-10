/*************************************************************************
	> File Name: pp_refr.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sun Jun  7 20:35:00 2026
	> Description: Asynchronous-like task execution handling slicing and drawing trees.
 ************************************************************************/

#include "core/pp_refr.h"
#include "draw/pp_area_dsc_draw.h"
#include "core/pp_obj.h"
#include <string.h>

#ifndef PP_REFR_INFO
#define PP_REFR_INFO(fmt,...) fprintf(stdout,"REFR INFO %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#define PP_REFR_ERROR(fmt,...) fprintf(stderr,"REFR INFO %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#endif // PP_REFR_INFO

/* Internal subroutines clone directly matching lv_refr.c core logic mapping */
static void pp_refr_join_area(void);
static void pp_refr_invalid_areas(void);
static void pp_refr_area(const pp_area_t * area_p);
static void pp_refr_obj_and_children(pp_canvas_t * canvas, pp_obj_t * obj, const pp_area_t * clip_area);

void pp_display_refr_timer(void)
{
    if (global_display && global_display->inv_p != 0) {
		// Phase 1: Optimize overlap bounding layouts
		pp_refr_join_area();

		// Phase 2: Consume active ledger updates through cutting processors
		pp_refr_invalid_areas();

		// Phase 3: Free the cache list slots down
		memset(global_display->inv_areas, 0, sizeof(global_display->inv_areas));
		memset(global_display->inv_area_joined, 0, sizeof(global_display->inv_area_joined));
		global_display->inv_p = 0;
	}

	pp_canvas_change_foreground_point(global_display->canvas);
}

static void pp_refr_join_area(void)
{
    uint32_t join_from;
    uint32_t join_in;
    pp_area_t joined_area;

    for (join_in = 0; join_in < global_display->inv_p; join_in++) {
        if (global_display->inv_area_joined[join_in] != 0) continue;

        for (join_from = 0; join_from < global_display->inv_p; join_from++) {
            if (global_display->inv_area_joined[join_from] != 0 || join_in == join_from) {
                continue;
            }

            // Detect if they clip into each other
            if (pp_area_is_on(&global_display->inv_areas[join_in], &global_display->inv_areas[join_from])) {
                pp_area_join(&joined_area, &global_display->inv_areas[join_in], &global_display->inv_areas[join_from]);

                // Pack only if size configuration gains computational speed advantages
                if (pp_area_get_size(&joined_area) < (pp_area_get_size(&global_display->inv_areas[join_in]) + pp_area_get_size(&global_display->inv_areas[join_from]))) {
                    global_display->inv_areas[join_in] = joined_area;
                    global_display->inv_area_joined[join_from] = 1; 
                }
            }
        }
    }
}

static void pp_refr_invalid_areas(void)
{
    for (uint32_t i = 0; i < global_display->inv_p; i++) {
        if (global_display->inv_area_joined[i]) continue;

        pp_area_t inv_a = global_display->inv_areas[i];

        if (global_display->render_mode == PP_RENDER_MODE_PARTIAL) {
            // Horizontal row banding engine supporting constrained hardware memory
            int32_t row = inv_a.y1;
            int32_t max_row = (int32_t)global_display->max_partial_rows;

            while (row <= inv_a.y2) {
                pp_area_t sub_area;
                sub_area.x1 = inv_a.x1;
                sub_area.x2 = inv_a.x2;
                sub_area.y1 = row;
                sub_area.y2 = row + max_row - 1;

                if (sub_area.y2 > inv_a.y2) sub_area.y2 = inv_a.y2;

                // Fire rendering pipeline over this single window strip chunk
                pp_refr_area(&sub_area);

                // Simulate hardware buffer flush execution hook here (like draw_buf_flush)
                row += max_row;
            }
        } else {
            pp_refr_area(&inv_a);
        }
    }
}

static void pp_refr_area(const pp_area_t * area_p)
{
    // Point core drawer downstream targeting root layout trees bound inside the sub-slice clip window
    pp_refr_obj_and_children(global_display->canvas, global_display->root_obj, area_p);
}


static void pp_refr_obj_and_children(pp_canvas_t * canvas, pp_obj_t * obj, const pp_area_t * clip_area)
{
	if (!obj || !clip_area || !canvas) return;

	pp_area_t draw_box;
	if (!pp_area_intersect(&draw_box, &obj->coords, clip_area)) {
		return; 
	}

	pp_draw_dsc_t draw_dsc;
	pp_draw_dsc_init(&draw_dsc);

	const pp_style_t * style = pp_obj_get_style(obj);
	bool is_rect = true;
	if (style) {
		if(style->img_src){
			is_rect = false;
			draw_dsc.src = style->img_src;
			PP_REFR_INFO("The obj is image, draw...");
		} else {
			draw_dsc.color = style->bg_color;
			draw_dsc.radius = style->radius;
		}
	} else {
		draw_dsc.color = MY_STYLE->bg_color;
		draw_dsc.radius = MY_STYLE->radius;
	}

	if(is_rect) pp_canvas_draw_rect(canvas, &draw_box, &draw_dsc);
	else pp_canvas_draw_image(canvas,&obj->coords,&draw_box,&draw_dsc);

	size_t child_cnt = pp_obj_get_child_cnt(obj);
	for (size_t i = 0; i < child_cnt; i++) {
		pp_obj_t * child = pp_obj_get_child_by_index(obj, i);
		if (child) {
			pp_refr_obj_and_children(canvas, child, clip_area);
		}
	}
}
