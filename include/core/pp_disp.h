/*************************************************************************
	> File Name: pp_disp.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sun Jun  7 20:15:00 2026
	> Description: Display device core managing device states and dirty ledgers.
 ************************************************************************/

#ifndef PP_DISP_H
#define PP_DISP_H

#include "draw/pp_canvas.h"
#include "core/pp_obj.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef PP_DISP_INFO
#include <stdio.h>
#define PP_DISP_INFO(fmt,...)  fprintf(stdout, "DISP INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_DISP_ERROR(fmt,...) fprintf(stderr, "DISP ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PP_RENDER_MODE_FULL,      
    PP_RENDER_MODE_PARTIAL    
} pp_render_mode_t;

typedef struct _pp_disp_s{
    pp_canvas_t * canvas;             // Hardware frame buffer binding
    pp_obj_t    * root_obj;           // Root node anchor anchoring the entire layout tree
    int32_t       hor_res;            // Active horizontal resolution
    int32_t       ver_res;            // Active vertical resolution
    
    /* Invalidation tracking ledger matching the LV_INV_BUF_SIZE design */
    pp_area_t     inv_areas[32];      // Collected dirty coordinates cache
    uint8_t       inv_area_joined[32];// Joined whole indicator flags
    uint32_t      inv_p;              // Valid entries inside invalid cache
    
    pp_render_mode_t render_mode;     // Active buffer flush logic
    uint32_t         max_partial_rows;// Slicing stride limit for partial mode
} pp_disp_t;

/* Global instance anchor matching disp_def system */
pp_disp_t * pp_disp_get_instance(void);

/* Lifecycle and Invalidation API */
bool pp_disp_init(int32_t hor_res, int32_t ver_res, pp_render_mode_t mode, uint32_t max_rows);
void pp_disp_deinit(void);
void pp_disp_invalidate_area(const pp_area_t * area_p);

#ifdef __cplusplus
}
#endif

#endif /* PP_DISP_H */
