/*************************************************************************
	> File Name: pp_show.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sat 13 Jun 2026 01:11:03 AM CST
 ************************************************************************/

#include "display/pp_show.h"
#include <pthread.h>
#ifndef PP_SHOW_INFO
#define PP_SHOW_INFO(fmt,...)  fprintf(stdout,"SHOW INFO %s %d :"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#define PP_SHOW_ERROR(fmt,...) fprintf(stderr,"SHOW ERROR %s %d :"fmt"\n",__func__,__LINE__,##__VA_ARGS__)
#endif // PP_SHOW_INFO

static FILE * g_ffplay_pipe = NULL;

static void pp_uv_push_frame_cb(uv_timer_t *handle)
{
	PP_ASSERT(handle);
	pp_disp_t * disp = pp_disp_get_instance();
	PP_ASSERT(disp && disp->canvas);

	bool do_swap = false;
	uv_mutex_lock(&disp->lock);
	if(disp->render_complete){
		disp->render_complete = false;
		do_swap = true;
	}
	disp->v_sync_ready = true;
	uv_mutex_unlock(&disp->lock);

	pp_canvas_t * canvas = disp->canvas;
	size_t frame_bytes = canvas->buffer_size;

	if(do_swap){
		pp_canvas_change_foreground_point(canvas);
	}

	pthread_mutex_lock(&canvas->ptr_mutex);
	uint8_t * local_flush_ptr = PP_CANVAS_GET_FG(canvas);
	pthread_mutex_unlock(&canvas->ptr_mutex);

	if(g_ffplay_pipe && local_flush_ptr){
		fwrite(local_flush_ptr, 1, frame_bytes, g_ffplay_pipe);
		fflush(g_ffplay_pipe);
	}

	uv_async_send(&disp->async_to_ui);
}

void on_ui_complete_signal(uv_async_t * handle)
{
	pp_disp_t * disp = pp_disp_get_instance();
	PP_ASSERT(disp && handle);
	// TODO 
}

void * pp_ffplay_consumer_thread(void *arg)
{

	pp_disp_t * disp = pp_disp_get_instance();
	PP_ASSERT(disp && disp->canvas);
	pp_canvas_t * canvas = disp->canvas;

	char cmd[256];
	snprintf(cmd, sizeof(cmd), "ffplay -f rawvideo -pixel_format rgb24 -video_size %dx%d -framerate 60 -i -",canvas->width,canvas->height);

	g_ffplay_pipe = popen(cmd, "w");
	if(!g_ffplay_pipe){
		PP_SHOW_ERROR("Failed to open ffplay pipeline! Device simulation aborted.");
		return arg;
	}
	PP_SHOW_INFO("FFplay deployed successfully (%d%d@60Hz).",canvas->width,canvas->height);

	/*oOoOoOoOoOoOoOoOoOoOoO
	    uv loop go go go
	 oOoOoOoOoOoOoOoOoOoOoOo*/
	uv_loop_t * disp_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
	PP_ASSERT(disp_loop);
	if(uv_loop_init(disp_loop)<0){
		PP_SHOW_ERROR("OOM! Failed to init uv loop.");
		pclose(g_ffplay_pipe);
		return arg;
	}
	disp->display_loop = disp_loop;

	uv_async_init(disp_loop,&disp->async_to_display,on_ui_complete_signal);

	uv_timer_t disp_timer_handle;
	uv_timer_init(disp_loop, &disp_timer_handle);

	uv_timer_start(&disp_timer_handle, pp_uv_push_frame_cb, 16, 16);

	uv_run(disp_loop, UV_RUN_DEFAULT);

	uv_loop_close(disp_loop);
	free(disp_loop);
	if(disp->display_loop) disp->display_loop = NULL;
	if(g_ffplay_pipe){
		pclose(g_ffplay_pipe);
		g_ffplay_pipe = NULL;
	}
	return arg;
}

void pp_show_engine_start(void)
{
	PP_FFPLAY_PIPELINE_LAUNCH(NULL);
}

