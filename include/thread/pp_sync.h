/*************************************************************************
	> File Name: pp_sync.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Tue 16 Jun 2026 10:18:54 AM CST
 ************************************************************************/

#ifndef PP_SYNC_H
#define PP_SYNC_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile int32_t active_tasks;
}pp_sync_t;

void pp_sync_init(pp_sync_t * sync);
void pp_sync_inc(pp_sync_t * sync);
void pp_sync_dec(pp_sync_t * sync);
void pp_sync_wait_zero(pp_sync_t * sync);

#endif // PP_SYNC_H

#if defined(PP_SYNC_IMPLEMENTATION) && !defined(PP_SYNC_IMPLEMENTATION_DOWN)
#define PP_SYNC_IMPLEMENTATION_DOWN

void pp_sync_init(pp_sync_t * sync)
{
	pthread_mutex_init(&sync->mutex, NULL);
	pthread_cond_init(&sync->cond, NULL);
	sync->active_tasks = 0;
}

void pp_sync_inc(pp_sync_t * sync)
{
	pthread_mutex_lock(&sync->mutex);
	sync->active_tasks++;
	pthread_mutex_unlock(&sync->mutex);
}

void pp_sync_dec(pp_sync_t * sync)
{
	pthread_mutex_lock(&sync->mutex);
	sync->active_tasks--;

	if(sync->active_tasks == 0){
		pthread_cond_broadcast(&sync->cond);
	}
	pthread_mutex_unlock(&sync->mutex);
}

void pp_sync_wait_zero(pp_sync_t * sync)
{
	pthread_mutex_lock(&sync->mutex);

	while(sync->active_tasks > 0){
		pthread_cond_wait(&sync->cond, &sync->mutex);
	}

	pthread_mutex_unlock(&sync->mutex);
}


#endif // PP_SYNC_IMPLEMENTATION
