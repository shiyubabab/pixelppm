/*************************************************************************
	> File Name: pp_obj.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 16:18:50 2026
	> Description: Object operations core supporting topology, geometry, and styles.
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <core/pp_obj.h>
#include <core/pp_obj_class.h>
#include <helper/pp_mem.h>

#define MY_CLASS &pp_obj_class

static void pp_obj_constructor(const pp_obj_class_t * class_p, pp_obj_t * obj);
static void pp_obj_destructor(const pp_obj_class_t * class_p, pp_obj_t * obj);
static void pp_obj_add_child_to_parent(pp_obj_t * parent, pp_obj_t * child);
static pp_style_t * pp_obj_ensure_local_style(pp_obj_t * obj);

const pp_obj_class_t pp_obj_class = {
    .base_class    = NULL,
    .constructor_cb = pp_obj_constructor,
    .destructor_cb  = pp_obj_destructor,
    .user_data      = NULL,
    .name           = "obj",
    .width_def      = 100,
    .height_def     = 45,
    .instance_size  = sizeof(pp_obj_t)
};

const pp_style_t pp_obj_style = {
	.bg_color = {0,0,0},
	.radius = 0,
	.bg_opa = 255
};

pp_obj_t * pp_obj_create(pp_obj_t * parent)
{
    pp_obj_t * obj = pp_obj_class_create_obj(MY_CLASS, parent);
    if (!obj) return NULL;
    
    pp_obj_class_init_obj(obj);
    
    return obj;
}

void pp_obj_allocate_spec_attr(pp_obj_t * obj)
{
    if (obj && obj->spec_attr == NULL) {
        obj->spec_attr = (_pp_obj_spec_attr_t *)pp_malloc(sizeof(_pp_obj_spec_attr_t));
        if (!obj->spec_attr) {
            PP_OBJ_ERROR("OOM block cluster during spec_attr allocation.");
            return;
        } else {
			PP_OBJ_INFO("obj spec_attr alloc successful.");
		}
        memset(obj->spec_attr, 0, sizeof(_pp_obj_spec_attr_t));
    }
}

pp_obj_t * pp_obj_get_parent(const pp_obj_t * obj)
{
    return obj ? obj->parent : NULL;
}

size_t pp_obj_get_child_cnt(const pp_obj_t * obj)
{
    if (!obj || !obj->spec_attr) return 0;
    return obj->spec_attr->child_cnt;
}

pp_obj_t * pp_obj_get_child_by_index(const pp_obj_t * obj, size_t index)
{
    if (!obj || !obj->spec_attr || index >= obj->spec_attr->child_cnt) return NULL;
    return obj->spec_attr->children[index];
}

void pp_obj_set_pos(pp_obj_t * obj, int32_t x, int32_t y)
{
	pp_obj_t * p = pp_obj_get_parent(obj);
    if (!obj || !p) return;
    int32_t w = pp_area_get_width(&obj->coords);
    int32_t h = pp_area_get_height(&obj->coords);

	int32_t px =  p->coords.x1;
	int32_t py =  p->coords.y1;

    obj->coords.x1 = px + x;
    obj->coords.y1 = py + y;
    obj->coords.x2 = px + x + w - 1;
    obj->coords.y2 = py + y + h - 1;
}

void pp_obj_set_size(pp_obj_t * obj, int32_t w, int32_t h)
{
    if (!obj) return;
    if (w < 0) w = 0;
    if (h < 0) h = 0;

    obj->coords.x2 = obj->coords.x1 + w - 1;
    obj->coords.y2 = obj->coords.y1 + h - 1;
}

void pp_obj_set_bg_color(pp_obj_t * obj, pp_color_t color)
{
    if (!obj) return;

	pp_style_t * local_s = pp_obj_ensure_local_style(obj);
	if(!local_s) return ;

	PP_OBJ_INFO("set obj color: r %d g %d b %d",color.r,color.g,color.b);
	local_s->bg_color = color;
}


void pp_obj_set_radius(pp_obj_t * obj, int32_t radius)
{
    if (!obj) return;

	pp_style_t * local_s = pp_obj_ensure_local_style(obj);
	if(!local_s) return ;

	local_s->radius = radius;
}


int32_t pp_obj_get_width(const pp_obj_t * obj)
{
    return obj ? pp_area_get_width(&obj->coords) : 0;
}

int32_t pp_obj_get_height(const pp_obj_t * obj)
{
    return obj ? pp_area_get_height(&obj->coords) : 0;
}

void pp_obj_set_style(pp_obj_t * obj,const pp_style_t * style)
{
    if (!obj) return;
	obj->style = (pp_style_t *)style;
}

pp_style_t * pp_obj_get_style(const pp_obj_t * obj)
{
	return obj? obj->style:NULL;
}

void pp_obj_set_image_src(pp_obj_t * obj, const pp_image_t * img_src)
{
    if (!obj) return;

	pp_style_t * local_s = pp_obj_ensure_local_style(obj);
	if(!local_s) return ;

	if(!local_s->img_src) local_s->img_src = img_src;

}

static void pp_obj_constructor(const pp_obj_class_t * class_p, pp_obj_t * obj)
{
    PP_OBJ_INFO("Basic Object initialization injected: '%s'", class_p->name);
    obj->coords.x1 = 0;
    obj->coords.y1 = 0;
    obj->coords.x2 = (int16_t)(class_p->width_def - 1);
    obj->coords.y2 = (int16_t)(class_p->height_def - 1);
    obj->user_data = NULL;
}

static void pp_obj_destructor(const pp_obj_class_t * class_p, pp_obj_t * obj)
{
    PP_OBJ_INFO("Recycling bottom layer allocations for object: '%s'", class_p->name);
	if(obj->style && obj->style != MY_STYLE){
		pp_free(obj->style);
		obj->style = NULL;
	}

    if (obj->spec_attr) {
        if (obj->spec_attr->children) {
            pp_free(obj->spec_attr->children);
        }
        pp_free(obj->spec_attr);
        obj->spec_attr = NULL;
    }
}

static void pp_obj_add_child_to_parent(pp_obj_t * parent, pp_obj_t * child)
{
    if (!parent || !child) return;
    
    pp_obj_allocate_spec_attr(parent);
    _pp_obj_spec_attr_t * attr = parent->spec_attr;
    if (!attr) return;

    size_t new_cnt = attr->child_cnt + 1;
    pp_obj_t ** new_children = (pp_obj_t **)pp_realloc(attr->children, new_cnt * sizeof(pp_obj_t *));
    if (new_children) {
        attr->children = new_children;
        attr->children[attr->child_cnt] = child;
        attr->child_cnt = new_cnt;
    } else {
        PP_OBJ_ERROR("Critical allocation crash linking child topology node.");
    }
}

static pp_style_t * pp_obj_ensure_local_style(pp_obj_t * obj)
{
	if(!obj) return NULL;

	if(obj->style == NULL || obj->style == MY_STYLE){
		pp_style_t * local_style = (pp_style_t *)pp_malloc(sizeof(pp_style_t));
		if(!local_style){
			PP_OBJ_ERROR("Fail to alloc a local style");
			return NULL;
		}

		if(obj->style == MY_STYLE){
			*local_style = *MY_STYLE;
		} else {
			local_style->bg_color = PP_COLOR_HEX(0x000000);
			local_style->radius = 0;
			local_style->bg_opa = 255;
		}

		obj->style = local_style;
	}
	return obj->style;
}
