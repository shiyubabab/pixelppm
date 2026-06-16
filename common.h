#ifndef PP_COMMON_H
#define PP_COMMON_H

#define X86_ARCH
#define PP_MEM_IMPLEMENTATION
#define PP_AREA_IMPLEMENTATION
#include <helper/pp_mem.h>
#include <helper/pp_area.h>

#define PP_CANVAS_IMPLEMENTATION
#define PP_AREA_DSC_DRAW_IMPLEMENTATION
#include <draw/pp_canvas.h>
#include <draw/pp_area_dsc_draw.h>

#define PP_UV_LOOP_IMPLEMENTATION
#include <framework/pp_uv_loop.h>

#define PP_THREAD_IMPLEMENTATION
#define PP_QUEUE_IMPLEMENTATION
#define PP_TASK_IMPLEMENTATION
#define PP_POOL_IMPLEMENTATION
#define PP_SYNC_IMPLEMENTATION
#include <thread/pp_thread.h>
#include <thread/pp_queue.h>
#include <thread/pp_task.h>
#include <thread/pp_pool.h>
#include <thread/pp_sync.h>

#include <core/pp_obj.h>
#include <core/pp_disp.h>
#include <core/pp_refr.h> 
#include <display/pp_show.h> 

#ifndef PP_MAIN_INFO 
#include <stdio.h>
#define PP_MAIN_INFO(fmt,...)  fprintf(stdout, "MAIN INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_MAIN_ERROR(fmt,...) fprintf(stderr, "MAIN ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

#endif // PP_COMMON_H
