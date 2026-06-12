/*************************************************************************
	> File Name: pp_types.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 15:04:39 2026
 ************************************************************************/


#ifndef PP_TYPES_H
#define PP_TYPES_H

#ifndef PP_ASSERT
#include <assert.h>
#define PP_ASSERT assert
#endif

struct _pp_obj_s;
typedef struct _pp_obj_s pp_obj_t;

struct _pp_obj_class_s;
typedef struct _pp_obj_class_s pp_obj_class_t;

struct _pp_disp_s;
typedef struct _pp_disp_s pp_disp_t;

#endif // PP_TYPES_H
