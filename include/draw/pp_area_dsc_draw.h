/*************************************************************************
	> File Name: pp_area_dsc_draw.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sat Jun  6 18:05:00 2026
	> Description: Canvas-oriented shape renderer driven by pp_draw_dsc_t.
 ************************************************************************/

#ifndef PP_AREA_DSC_DRAW_H
#define PP_AREA_DSC_DRAW_H

#include <helper/pp_area.h>
#include "pp_canvas.h" // Bind cleanly with the newly established pp_canvas_t
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PP_DSC_DRAW_INFO
#define PP_DSC_DRAW_INFO(fmt,...) fprintf(stdout,"DRAW INFO %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#define PP_DSC_DRAW_ERROR(fmt,...) fprintf(stderr,"DRAW ERROR %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#endif

/*oOoOoOoOoOoOoOoOoOoO
 * DATA STRUCTURES
 *oOoOoOoOoOoOoOoOoOoO*/

// Color structure aligned with standard 24-bit RGB
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pp_color_t;

#define PP_COLOR_HEX(hex) ((pp_color_t){ \
    .r = (uint8_t)(((hex) >> 16) & 0xFF), \
    .g = (uint8_t)(((hex) >> 8 ) & 0xFF), \
    .b = (uint8_t)(((hex) >> 0 ) & 0xFF)  \
})

typedef struct {
	const pp_color_t *pixel_map;
	int32_t width;
	int32_t height;
} pp_image_t;

// Unified Shape Descriptor Structure (The "How to Draw" properties)
typedef struct {
    pp_color_t color;       // Base fill color
    int32_t radius;         // Corner radius for rectangles / circle radius
    int16_t start_angle;    // For arc and sector (0-360)
    int16_t end_angle;      // For arc and sector (0-360)
    bool is_filled;         // True for solid fill, false for outline/wireframe
	const pp_image_t * src;
} pp_draw_dsc_t;

// Triangle coordinate parameters
typedef struct {
    int32_t x1; int32_t y1;
    int32_t x2; int32_t y2;
    int32_t x3; int32_t y3;
} pp_triangle_t;

/*oOoOoOoOoOoOoOoOoOoO
 * GLOBAL PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/

// Standard Initializer to secure safe default style states
static inline void pp_draw_dsc_init(pp_draw_dsc_t * dsc) {
    if (dsc) {
        dsc->color.r = 0; dsc->color.g = 0; dsc->color.b = 0;
        dsc->radius = 0;
        dsc->start_angle = 0; dsc->end_angle = 360;
        dsc->is_filled = true;
    }
}

// Canvas-oriented geometric shape drawers
void pp_canvas_draw_rect(pp_canvas_t * canvas, const pp_area_t * coords, const pp_draw_dsc_t * dsc);
void pp_canvas_draw_circle(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc);
void pp_canvas_draw_arc(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc);
void pp_canvas_draw_sector(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc);
void pp_canvas_draw_image(pp_canvas_t * canvas,const pp_area_t * obj_coords , const pp_area_t * coords, const pp_draw_dsc_t * dsc);
void pp_canvas_draw_triangle(pp_canvas_t * canvas, const pp_triangle_t * tri, const pp_draw_dsc_t * dsc);

// Embedded core watermark printer linked to a target canvas
void pp_canvas_draw_watermark(pp_canvas_t * canvas, const char * text);

#ifdef __cplusplus
}
#endif

#endif /* PP_AREA_DSC_DRAW_H */


/*oOoOoOoOoOoOoOoOoOoO
 * IMPLEMENTATION PART
 *oOoOoOoOoOoOoOoOoOoO*/
#if defined(PP_AREA_DSC_DRAW_IMPLEMENTATION) && !defined(PP_AREA_DSC_DRAW_IMPLEMENTATION_DOWN)
#define PP_AREA_DSC_DRAW_IMPLEMENTATION_DOWN


#include <math.h>

// Micro internal pixel blitter bounded strictly to the given canvas context
static inline void pp_draw_pixel_to_canvas(pp_canvas_t * canvas, int32_t x, int32_t y, pp_color_t color) 
{
    if (x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
        size_t idx = (y * canvas->width + x) * 3;
        canvas->buffer[idx + 0] = color.r;
        canvas->buffer[idx + 1] = color.g;
        canvas->buffer[idx + 2] = color.b;
    }
}

/**
 * @brief Descriptor-Based Rectangle (Supports standard and rounded variants)
 */
void pp_canvas_draw_rect(pp_canvas_t * canvas, const pp_area_t * coords, const pp_draw_dsc_t * dsc)
{
    if (!canvas || !coords || !dsc || !canvas->buffer) return;

	PP_DSC_DRAW_INFO("draw rect : x1 %d y1 %d x2 %d y2 %d",coords->x1,coords->y1,coords->x2,coords->y2);
    // Boundary optimization: Only sweep loops within the bounded coordinates
    for (int32_t y = coords->y1; y <= coords->y2; y++) {
        for (int32_t x = coords->x1; x <= coords->x2; x++) {
            pp_point_t p = {x, y};
            // Call your established high-performance pp_area_is_point_on for screening
            if (pp_area_is_point_on(coords, &p, dsc->radius)) {
                pp_draw_pixel_to_canvas(canvas, x, y, dsc->color);
            }
        }
    }
}

/**
 * @brief Descriptor-Based Circle (Treated as a perfectly degenerated rounded rect)
 */
void pp_canvas_draw_circle(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc)
{
    if (!canvas || !dsc) return;
    
    pp_area_t area = { cx - dsc->radius, cy - dsc->radius, cx + dsc->radius, cy + dsc->radius };
    pp_draw_dsc_t circle_dsc = *dsc;
    circle_dsc.radius = dsc->radius; 

    pp_canvas_draw_rect(canvas, &area, &circle_dsc);
}

/**
 * @brief Descriptor-Based Unfilled Arc Outline (Bresenham)
 */
void pp_canvas_draw_arc(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc)
{
    if (!canvas || !dsc || !canvas->buffer) return;
    
    int32_t x = dsc->radius, y = 0;
    int32_t P = 1 - dsc->radius;

    while (x > y) {
        y++;
        if (P <= 0) P = P + 2 * y + 1;
        else { x--; P = P + 2 * y - 2 * x + 1; }

        pp_point_t pts[] = {
            {cx + x, cy - y}, {cx - x, cy - y}, {cx + x, cy + y}, {cx - x, cy + y},
            {cx + y, cy - x}, {cx - y, cy - x}, {cx + y, cy + x}, {cx - y, cy + x}
        };
        for(int i = 0; i < 8; i++) {
            int32_t dx = pts[i].x - cx, dy = pts[i].y - cy;
            int16_t angle = (int16_t)((atan2(dy, dx) * 180.0) / 3.14159265);
            if (angle < 0) angle += 360;
            if (angle >= dsc->start_angle && angle <= dsc->end_angle) {
                pp_draw_pixel_to_canvas(canvas, pts[i].x, pts[i].y, dsc->color);
            }
        }
    }
}

/**
 * @brief Descriptor-Based Filled Sector
 */
void pp_canvas_draw_sector(pp_canvas_t * canvas, int32_t cx, int32_t cy, const pp_draw_dsc_t * dsc)
{
    if (!canvas || !dsc || !canvas->buffer) return;
    
    int32_t r2 = dsc->radius * dsc->radius;
    pp_area_t box = { cx - dsc->radius, cy - dsc->radius, cx + dsc->radius, cy + dsc->radius };

    for (int32_t y = box.y1; y <= box.y2; y++) {
        for (int32_t x = box.x1; x <= box.x2; x++) {
            int32_t dx = x - cx, dy = y - cy;
            if ((uint32_t)(dx * dx + dy * dy) <= (uint32_t)r2) {
                int16_t angle = (int16_t)((atan2(dy, dx) * 180.0) / 3.14159265);
                if (angle < 0) angle += 360;
                if (angle >= dsc->start_angle && angle <= dsc->end_angle) {
                    pp_draw_pixel_to_canvas(canvas, x, y, dsc->color);
                }
            }
        }
    }
}

void pp_canvas_draw_image(pp_canvas_t * canvas,const pp_area_t * obj_coords , const pp_area_t * coords, const pp_draw_dsc_t * dsc)
{
	if (!canvas || !dsc || !canvas->buffer || !dsc->src || !obj_coords || !coords) return;

	const pp_image_t * img = dsc->src;
	int32_t iw = img->width;
	int32_t ih = img->height;
	int32_t ix = obj_coords->x1;
	int32_t iy = obj_coords->y1;

	pp_area_t draw_box;
	if(!pp_area_intersect(&draw_box, obj_coords, coords)) return;

	pp_area_t image_box = { ix, iy , ix + iw - 1, iy + ih - 1 };
	if(!pp_area_intersect(&draw_box, &draw_box, &image_box)) return;


    for (int32_t y = draw_box.y1; y <= draw_box.y2; y++) {
        for (int32_t x = draw_box.x1; x <= draw_box.x2; x++) {

			int32_t img_x = x - obj_coords->x1;
			int32_t img_y = y - obj_coords->y1;

			uint32_t img_pixel_index = (uint32_t)img_y * (uint32_t)iw + (uint32_t)img_x;
			pp_color_t color = img->pixel_map[img_pixel_index];
			pp_draw_pixel_to_canvas(canvas, x, y, color);
        }
    }
}


/**
 * @brief Descriptor-Based Filled Triangle (Scanline Rasterization)
 */
void pp_canvas_draw_triangle(pp_canvas_t * canvas, const pp_triangle_t * tri, const pp_draw_dsc_t * dsc)
{
    if (!canvas || !tri || !dsc || !canvas->buffer) return;
    
    pp_point_t v[] = { {tri->x1, tri->y1}, {tri->x2, tri->y2}, {tri->x3, tri->y3} };
    
    if (v[0].y > v[1].y) { pp_point_t t = v[0]; v[0] = v[1]; v[1] = t; }
    if (v[0].y > v[2].y) { pp_point_t t = v[0]; v[0] = v[2]; v[2] = t; }
    if (v[1].y > v[2].y) { pp_point_t t = v[1]; v[1] = v[2]; v[2] = t; }

    #define PP_DSC_INTERP(y, x1, y1, x2, y2) ((int32_t)(x1) + (int32_t)(((int32_t)(x2) - (int32_t)(x1)) * ((int32_t)(y) - (int32_t)(y1))) / ((int32_t)(y2) - (int32_t)(y1)))

    int32_t yc;
    if (v[0].y != v[1].y) {
        for (yc = v[0].y; yc <= v[1].y; yc++) {
            int32_t sx = PP_DSC_INTERP(yc, v[0].x, v[0].y, v[1].x, v[1].y);
            int32_t ex = PP_DSC_INTERP(yc, v[0].x, v[0].y, v[2].x, v[2].y);
            if (sx > ex) { int32_t t = sx; sx = ex; ex = t; }
            for (int32_t x = sx; x <= ex; x++) pp_draw_pixel_to_canvas(canvas, x, yc, dsc->color);
        }
    }
    if (v[1].y != v[2].y) {
        for (yc = v[1].y; yc <= v[2].y; yc++) {
            int32_t sx = PP_DSC_INTERP(yc, v[1].x, v[1].y, v[2].x, v[2].y);
            int32_t ex = PP_DSC_INTERP(yc, v[0].x, v[0].y, v[2].x, v[2].y);
            if (sx > ex) { int32_t t = sx; sx = ex; ex = t; }
            for (int32_t x = sx; x <= ex; x++) pp_draw_pixel_to_canvas(canvas, x, yc, dsc->color);
        }
    }
    #undef PP_DSC_INTERP
}


/**
 * @brief Watermark Generator stamped cleanly onto the target canvas
 */
void pp_canvas_draw_watermark(pp_canvas_t * canvas, const char * text)
{
    if(!canvas || !canvas->buffer || !text) return;
    
    static const uint8_t min_font[128][8] = {
        ['A'] = {24,36,66,126,66,66,66,0},  ['B'] = {60,66,66,62,66,66,60,0},
        ['D'] = {60,66,66,66,66,66,60,0},   ['E'] = {126,64,64,120,64,64,126,0},
        ['H'] = {66,66,66,126,66,66,66,0},  ['I'] = {126,24,24,24,24,24,126,0},
        ['M'] = {66,102,90,66,66,66,66,0},  ['O'] = {60,66,66,66,66,66,60,0},
        ['R'] = {62,66,66,62,80,72,68,0},   ['S'] = {62,64,64,60,2,2,62,0},
        ['T'] = {126,24,24,24,24,24,24,0},  ['X'] = {66,66,36,24,36,66,66,0},
        ['_'] = {0,0,0,0,0,0,0,255}
    };
    
    int32_t px = 12, py = canvas->height - 22;
    pp_color_t white = {255, 255, 255};
    
    while (*text) {
        uint8_t ch = (uint8_t)*text;
        const uint8_t *mask = min_font[ch & 0x7F];
        for (int r = 0; r < 8; r++) {
            uint8_t row_bits = mask[r];
            for (int c = 0; c < 8; c++) {
                if (row_bits & (128 >> c)) {
                    pp_draw_pixel_to_canvas(canvas, px + c, py + r, white);
                }
            }
        }
        px += 9; text++;
    }
}

#endif /* PP_AREA_DSC_DRAW_IMPLEMENTATION */
