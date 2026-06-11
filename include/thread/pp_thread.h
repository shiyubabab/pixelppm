/*************************************************************************
	> File Name: pp_thread.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun 11 14:43:28 2026
 ************************************************************************/

#ifndef PP_THREAD_H
#define PP_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <errno.h>

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

#define handle_error(en, msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)

#ifndef PP_THREAD_INFO
#include <stdio.h>
#define PP_THREAD_INFO(fmt,...)  fprintf(stdout,"THREAD INFO:" fmt"\n",##__VA_ARGS__)
#define PP_THREAD_ERROR(fmt,...) fprintf(stderr,"THREAD ERROR:"fmt"\n",##__VA_ARGS__)
#endif // PP_THREAD_INFO

#define PP_FFPLAY_PIPELINE_LAUNCH(canvas_ptr) do { \
	pp_thread_attr_t _cfg = {					   \
		.name = "pp_ffplay_dma",                   \
		.stack_size = 64 * 1024,                   \
		.is_detach = true                          \
	};                                             \
	pthread_t _pid = pp_thread_create(pp_ffplay_consumer_thread, (void *)(canvas_ptr), &_cfg); \
	if (_pid == 0) PP_THREAD_ERROR("🚨 [CRITICAL] Crash initializing system asynchronous pipeline!");		\
} while(0)

#define PP_THREAD_LAUNCH(th_name, th_stack, th_func, th_arg) do {	\
	pp_thread_attr_t _cfg = {										\
		.name = th_name,											\
		.stack_size = th_stack,										\
		.is_detach = true											\
	};																\
	pthread_t _pid = pp_thread_create(th_func, th_arg, &_cfg);		\
	if (_pid == 0) PP_THREAD_ERROR("🚨 [CRITICAL] Crash initializing thread: %s!", th_name); \
} while(0)

typedef struct {
	const char * name;
	size_t		 stack_size;
	int32_t		 priority;
	bool		 is_detach;
} pp_thread_attr_t;

typedef void *(*pp_thread_func_t)(void *);

pthread_t pp_thread_create(pp_thread_func_t func, void * arg, const pp_thread_attr_t *attr);

#ifdef __cplusplus
}
#endif

#endif // PP_THREAD_H

#if defined(PP_THREAD_IMPLEMENTATION) && !defined(PP_THREAD_IMPLEMENTATION_DONE) 
#define PP_THREAD_IMPLEMENTATION_DONE

#ifdef __cplusplus
extern "C" {
#endif

pthread_t pp_thread_create(pp_thread_func_t func, void * arg, const pp_thread_attr_t *attr)
{
	pthread_t thread_id = 0;
	pthread_attr_t sys_attr;
	int32_t res;

	res = pthread_attr_init(&sys_attr);
	if(res != 0){
		handle_error_en(res, "Native attr init failed.");
		return 0;
	}

	pp_thread_attr_t default_attr = {
		.name = "pp_anonymous",
		.stack_size = 0,
		.priority = 0,
		.is_detach = true
	};

	const pp_thread_attr_t * p_attr = attr ? attr : &default_attr;

	if(p_attr->stack_size > 0){
		size_t min_stack = PTHREAD_STACK_MIN;
		size_t final_stack = p_attr->stack_size < min_stack ? min_stack : p_attr->stack_size;

		res = pthread_attr_setstacksize(&sys_attr, final_stack);
		if(res != 0) handle_error_en(res, "Custom stack size failed.");
	}

	if(p_attr->is_detach){
		res = pthread_attr_setdetachstate(&sys_attr, PTHREAD_CREATE_DETACHED);
		if(res != 0) handle_error_en(res, "Set detached state failed.");
	}

	res = pthread_create(&thread_id, &sys_attr, func, arg);

	pthread_attr_destroy(&sys_attr);

	if(res != 0){
		handle_error_en(res, "Target thread creation failed.");
		return 0;
	}

	if(p_attr->name){
		char truncated_name[16];
		strncpy(truncated_name, p_attr->name, sizeof(truncated_name) - 1);
		truncated_name[sizeof(truncated_name - 1)] = '\0';

		pthread_setname_np(thread_id, truncated_name);
	}

	return thread_id;
}

#ifdef __cplusplus
}
#endif

#endif // PP_THREAD_IMPLEMENTATION
