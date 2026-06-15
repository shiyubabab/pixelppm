/*************************************************************************
	> File Name: pp_pool.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Mon Jun 15 15:10:40 2026
 ************************************************************************/

#ifndef PP_POOL_H
#define PP_POOL_H

#include "pp_queue.h"
#include "pp_task.h"
#include "helper/pp_mem.h"

#ifndef PP_POOL_INFO
#define PP_POOL_INFO(fmt,...) fprintf(stdout,"POOL INFO %s %d :"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#define PP_POOL_ERROR(fmt,...) fprintf(stderr,"POOL ERROR %s %d :"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#endif // PP_POOL_INFO

typedef struct {
	pp_queue_t * task_queue;
	pthread_t  * threads;
	int32_t		 thread_count;
	volatile bool is_inited;
} pp_thread_pool_t;

pp_thread_pool_t * pp_thread_pool_get_instance(void);
bool pp_thread_pool_init(int32_t thread_count, int32_t queue_capacity);
bool pp_thread_pool_submit(pp_task_cb_t func, void *user_data);
void pp_thread_pool_destroy(void);


#endif // PP_POOL_H

#if defined(PP_POOL_IMPLEMENTATION) && !defined(PP_POOL_IMPLEMENTATION_DOWE)
#define PP_POOL_IMPLEMENTATION_DOWE

static pp_thread_pool_t g_thread_pool_instance = {
	.task_queue		= NULL,
	.threads		= NULL,
	.thread_count	= 0,
	.is_inited		= false
};

pp_thread_pool_t * pp_thread_pool_get_instance(void)
{
	if(!g_thread_pool_instance.is_inited){
		PP_POOL_ERROR("Accessed thread pool instance before physical init!");
		return NULL;
	}
	return &g_thread_pool_instance;
}

static void * pp_pool_worker_entry(void *arg)
{
	PP_ASSERT(arg == NULL);
	while(1) {
		pp_task_t * task = (pp_task_t *)pp_queue_pop(g_thread_pool_instance.task_queue);

		if(!task){
			break;
		}

		task->func(task->user_data);

		pp_task_destroy(task);
	}
	return NULL;
}

bool pp_thread_pool_init(int32_t thread_count, int32_t queue_capacity)
{
	if(g_thread_pool_instance.is_inited) return true;

	if(thread_count <= 0 || queue_capacity <= 0) return false;

	g_thread_pool_instance.thread_count = thread_count;

	g_thread_pool_instance.task_queue = pp_queue_create(queue_capacity);
	if(!g_thread_pool_instance.task_queue) {
		return false;
	}

	g_thread_pool_instance.threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
	if(!g_thread_pool_instance.threads){
		pp_queue_destroy(g_thread_pool_instance.task_queue);
		return false;
	}

	for( int32_t i = 0; i < thread_count; i++){
		pthread_create(&g_thread_pool_instance.threads[i], NULL, pp_pool_worker_entry, NULL);
	}

	g_thread_pool_instance.is_inited = true;

	PP_POOL_INFO("Thread pool successfully deployed with %d active worker.",thread_count);
	return true;
}

bool pp_thread_pool_submit(pp_task_cb_t func, void * user_data)
{
	if(!g_thread_pool_instance.is_inited || !func) return false;

	pp_task_t * task = pp_task_create(func, user_data);
	if(!task) return false;

	if(!pp_queue_push(g_thread_pool_instance.task_queue, task)){
		pp_task_destroy(task);
		return false;
	}

	return true;
}

void pp_thread_pool_destroy(void)
{
	if(!g_thread_pool_instance.is_inited) return;

	PP_POOL_INFO("Shutting down thread pool, reclaiming system resource...");

	pp_queue_destroy(g_thread_pool_instance.task_queue);

	for(int32_t i = 0; i < g_thread_pool_instance.thread_count; i++){
		pthread_join(g_thread_pool_instance.threads[i],NULL);
	}

	free(g_thread_pool_instance.threads);
	g_thread_pool_instance.threads = NULL;
	g_thread_pool_instance.task_queue = NULL;
	g_thread_pool_instance.thread_count = 0;
	g_thread_pool_instance.is_inited = false;

	PP_POOL_INFO("Thread pool destroyed.Clean exit.");
}

#endif // PP_POOL_IMPLEMENTATION
