/*************************************************************************
	> File Name: pp_obj_class.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 14:17:30 2026
 ************************************************************************/

#ifndef PP_OBJ_CLASS_H
#define PP_OBJ_CLASS_H

#include <core/pp_types.h>

struct _pp_obj_class_s {
	const pp_obj_class_t *base_class;
	void (*constructor_cb)(const pp_obj_class_t * class_p, pp_obj_t *obj);
	void (*destructor_cb)(const pp_obj_class_t * class_p, pp_obj_t *obj);
	// void (*event_cb)(const pp_obj_class_t * class_p, pp_event_t *e);

	void * user_data;
	const char *name;
	size_t width_def;
	size_t height_def;
	size_t instance_size;
};

pp_obj_t * pp_obj_class_create_obj(const pp_obj_class_t * class_p, pp_obj_t * parent);
void pp_obj_class_init_obj(pp_obj_t *obj);
void _pp_obj_destruct(pp_obj_t *obj);


#endif // PP_OBJ_CLASS_H
