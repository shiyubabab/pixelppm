/*************************************************************************
	> File Name: pp_task.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Mon 15 Jun 2026 02:12:06 PM CST
 ************************************************************************/

#ifndef PP_TASK_H
#define PP_TASK_H
#include <stdint.h>
#include "helper/pp_mem.h"

typedef void (*pp_task_cb_t)(void * user_data);

typedef struct {
	pp_task_cb_t func;
	void	* user_data;
} pp_task_t;

pp_task_t * pp_task_create(pp_task_cb_t func, void * user_data);

void pp_task_destroy(pp_task_t * task);

#endif // PP_TASK_H

#if defined(PP_TASK_IMPLEMENTATION) && !defined(PP_TASK_IMPLEMENTATION_DONW)
#define PP_TASK_IMPLEMENTATION_DONW

pp_task_t * pp_task_create(pp_task_cb_t func, void * user_data)
{
	if(!func) return NULL;

	pp_task_t * task = (pp_task_t *)pp_malloc(sizeof(pp_task_t));
	if(!task) return NULL;

	task->func = func;
	task->user_data = user_data;
	return task;
}

void pp_task_destroy(pp_task_t * task)
{
	if(task) {
		pp_free(task);
	}
}

#endif // PP_TASK_IMPLEMENTATION_DONW
