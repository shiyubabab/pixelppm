/*************************************************************************
	> File Name: pp_obj.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 14:17:30 2026
	> Description: Object Core Header with comprehensive layout and topology control.
 ************************************************************************/

#ifndef PP_OBJ_H
#define PP_OBJ_H

#include <stdio.h>
#include <core/pp_types.h>
#include <core/pp_obj_class.h>
#include <helper/pp_area.h>
#include <draw/pp_area_dsc_draw.h>

#ifndef PP_OBJ_INFO
#define PP_OBJ_INFO(fmt,...)  fprintf(stdout, "OBJ INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_OBJ_ERROR(fmt,...) fprintf(stderr, "OBJ ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

/*oOoOoOoOoOoOoOoOoOoO
 * DATA STRUCTURES
 *oOoOoOoOoOoOoOoOoOoO*/

typedef struct {
    pp_color_t bg_color;
    int32_t radius;
    uint8_t bg_opa;
	const pp_image_t * img_src;
} pp_style_t;

extern const pp_style_t pp_obj_style;
#define MY_STYLE (&pp_obj_style)

typedef struct {
    pp_obj_t ** children;
    size_t child_cnt;
} _pp_obj_spec_attr_t;

struct _pp_obj_s {
    const pp_obj_class_t * class_p;
    pp_obj_t * parent;
    _pp_obj_spec_attr_t * spec_attr;
    void * user_data;
    pp_area_t coords;
    pp_style_t * style;
    size_t is_deleting : 1;
};

/*oOoOoOoOoOoOoOoOoOoO
 * GLOBAL PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/

// Lifecycle Factory
pp_obj_t * pp_obj_create(pp_obj_t * parent);
void       pp_obj_del(pp_obj_t * obj);
void       pp_obj_allocate_spec_attr(pp_obj_t * obj);

// Topology Inspection
pp_obj_t * pp_obj_get_parent(const pp_obj_t * obj);
size_t     pp_obj_get_child_cnt(const pp_obj_t * obj);
pp_obj_t * pp_obj_get_child_by_index(const pp_obj_t * obj, size_t index);

// Geometric Dimension and Location Modification
void       pp_obj_set_pos(pp_obj_t * obj, int32_t x, int32_t y);
void       pp_obj_set_size(pp_obj_t * obj, int32_t w, int32_t h);
void       pp_obj_set_bg_color(pp_obj_t * obj, pp_color_t color);
void       pp_obj_set_radius(pp_obj_t * obj, int32_t radius);
int32_t    pp_obj_get_width(const pp_obj_t * obj);
int32_t    pp_obj_get_height(const pp_obj_t * obj);

// Style System Attachment
void       pp_obj_set_style(pp_obj_t * obj, const pp_style_t * style);
pp_style_t * pp_obj_get_style(const pp_obj_t * obj);
void       pp_obj_set_image_src(pp_obj_t * obj, const pp_image_t * img_src);

#endif // PP_OBJ_H
