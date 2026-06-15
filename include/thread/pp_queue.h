/*************************************************************************
	> File Name: pp_queue.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Mon 15 Jun 2026 10:51:33 AM CST
 ************************************************************************/

#ifndef PP_QUEUE_H
#define PP_QUEUE_H
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "helper/pp_mem.h"

typedef struct {
	void **data;
	int32_t		capacity;
	int32_t		head;
	int32_t		tail;
	int32_t		size;

	pthread_mutex_t mutex;
	pthread_cond_t	not_full;
	pthread_cond_t	not_empty;
	volatile bool	is_shutdown;
} pp_queue_t;

pp_queue_t * pp_queue_create(int32_t capacity);
void		 pp_queue_destroy(pp_queue_t * queue);
bool		 pp_queue_push(pp_queue_t * queue, void * item);
void *		 pp_queue_pop(pp_queue_t * queue);

#endif // PP_QUEUE_H

#if defined(PP_QUEUE_IMPLEMENTATION) && !defined(PP_QUEUE_IMPLEMENTATION_DOWN)
#define PP_QUEUE_IMPLEMENTATION_DOWN
#include <stdio.h>
#include <stdlib.h>

pp_queue_t * pp_queue_create(int32_t capacity)
{
	if(capacity <= 0) return NULL;

	pp_queue_t * queue = (pp_queue_t *)pp_malloc(sizeof(pp_queue_t));
	if(!queue) return NULL;

	queue->data = (void **)pp_malloc(sizeof(void *) * capacity);
	if(!queue->data){
		pp_free(queue);
		return NULL;
	}

	queue->capacity = capacity;
	queue->head		= 0;
	queue->tail		= 0;
	queue->size		= 0;
	queue->is_shutdown = false;

	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->not_full, NULL);
	pthread_cond_init(&queue->not_empty, NULL);

	return queue;
}

bool pp_queue_push(pp_queue_t * queue, void * item)
{
	if(!queue || queue->is_shutdown) return false;

	pthread_mutex_lock(&queue->mutex);

	while(queue->size == queue->capacity && !queue->is_shutdown){
		pthread_cond_wait(&queue->not_full, &queue->mutex);
	}

	if(queue->is_shutdown){
		pthread_mutex_unlock(&queue->mutex);
		return false;
	}

	queue->data[queue->tail] = item;
	queue->tail = (queue->tail + 1) % queue->capacity;
	queue->size++;

	pthread_cond_signal(&queue->not_empty);

	pthread_mutex_unlock(&queue->mutex);
	return true;
}

void * pp_queue_pop(pp_queue_t * queue)
{
	if(!queue || queue->is_shutdown) return NULL;

	pthread_mutex_lock(&queue->mutex);

	while(queue->size == 0 && !queue->is_shutdown){
		pthread_cond_wait(&queue->not_empty, &queue->mutex);
	}

	if(queue->is_shutdown && queue->size == 0){
		pthread_mutex_unlock(&queue->mutex);
		return NULL;
	}

	void * item = queue->data[queue->head];
	queue->head = (queue->head + 1) % queue->capacity;
	queue->size--;

	pthread_cond_signal(&queue->not_full);

	pthread_mutex_unlock(&queue->mutex);
	return item;
}

void pp_queue_destroy(pp_queue_t * queue)
{
	if(!queue) return;

	pthread_mutex_lock(&queue->mutex);
	queue->is_shutdown = true;

	pthread_cond_broadcast(&queue->not_full);
	pthread_cond_broadcast(&queue->not_empty);
	pthread_mutex_unlock(&queue->mutex);

	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->not_full);
	pthread_cond_destroy(&queue->not_empty);

	pp_free(queue->data);
	pp_free(queue);
}


#endif // PP_QUEUE_IMPLEMENTATION
