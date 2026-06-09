/*************************************************************************
	> File Name: pp_obj_class.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 15:18:57 2026
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <core/pp_obj.h>
#include <core/pp_obj_class.h>
#include <helper/pp_mem.h>

/*oOoOoOoOoOoOoOoO
 *	   DEFINE
 *oOoOoOoOoOoOoOoO*/
#define MY_CLASS &pp_obj_class

/*oOoOoOoOoOoOoOoOoOoO
 *	STATIC PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/
static void pp_obj_construct(const pp_obj_class_t * class_p, pp_obj_t *obj);
static size_t get_instance_size(const pp_obj_class_t * class_p);

/*oOoOoOoOoOoOoOoOoOoO
 *	GLOBAL FUNCTIONS
 *oOoOoOoOoOoOoOoOoOoO*/
pp_obj_t * pp_obj_class_create_obj(const pp_obj_class_t *class_p, pp_obj_t * parent)
{
	size_t s = get_instance_size(class_p);
	pp_obj_t * obj = (pp_obj_t *)pp_malloc(s);
	if(obj == NULL) return NULL;
	memset(obj,0,s);
	obj->class_p = class_p;
	obj->parent = parent;
	
	if(parent == NULL){
		// TODO pp_display_get_default();
	} else {
		if(parent->spec_attr == NULL){
			pp_obj_allocate_spec_attr(parent);
		}

		parent->spec_attr->child_cnt++;
		parent->spec_attr->children = pp_realloc(parent->spec_attr->children,
							sizeof(pp_obj_t *) * parent->spec_attr->child_cnt);
		if(parent->spec_attr->children == NULL){
			return NULL;
		}
		parent->spec_attr->children[parent->spec_attr->child_cnt - 1] = obj;
	}
	return obj;
}

void pp_obj_class_init_obj(pp_obj_t * obj)
{
	pp_obj_construct(obj->class_p, obj);
	pp_obj_t * parent = obj->parent;
	if(parent){
		// TODO pp_obj_send_event();
		// TODO pp_obj_invalidate();
	}
}

void _pp_obj_destruct(pp_obj_t * obj)
{
	if(obj->class_p->destructor_cb) obj->class_p->destructor_cb(obj->class_p,obj);

	if(obj->class_p->base_class){
		obj->class_p = obj->class_p->base_class;
		_pp_obj_destruct(obj);
	}
}


/*oOoOoOoOoOoOoOoOoOoO
 *	STATIC FUNCTIONS
 *oOoOoOoOoOoOoOoOoOoO*/
static void pp_obj_construct(const pp_obj_class_t * class_p, pp_obj_t *obj)
{
	if(obj->class_p->base_class){
		const pp_obj_class_t * original_class_p = obj->class_p;

		obj->class_p = obj->class_p->base_class;

		pp_obj_construct(class_p,obj);

		obj->class_p = original_class_p;
	}

	if(obj->class_p->constructor_cb) obj->class_p->constructor_cb(class_p,obj);
}

static size_t get_instance_size(const pp_obj_class_t * class_p)
{
	const pp_obj_class_t *base = class_p;
	while(base && base->instance_size == 0) base = base->base_class;

	if(base == NULL) return 0;

	return base->instance_size;
}
