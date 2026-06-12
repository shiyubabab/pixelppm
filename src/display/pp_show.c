/*************************************************************************
	> File Name: pp_show.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sat 13 Jun 2026 01:11:03 AM CST
 ************************************************************************/

#include "display/pp_show.h"

static void pp_uv_push_frame_cb(uv_timer_t *handle)
{
	PP_ASSERT(handle);
	pp_disp_t * disp = pp_disp_get_instance();
	PP_ASSERT(disp);
	pp_canvas_t * canvas = disp->canvas;
	PP_ASSERT(canvas);

	printf("OoOoOoOoOoOooOOoOOOOOoooOoOoOooOoo%d\n",PP_CANVAS_GET_FG(canvas)[0]);

}

void * pp_ffplay_consumer_thread(void *arg)
{
	uv_loop_t * loop = uv_default_loop();
	PP_ASSERT(loop);

	uv_timer_t timer_handle;
	uv_timer_init(loop, &timer_handle);

	uv_timer_start(&timer_handle, pp_uv_push_frame_cb, 16, 16);

	uv_run(loop, UV_RUN_DEFAULT);

	uv_loop_close(loop);
	return arg;
}

void pp_show_engine_start(void)
{
	PP_FFPLAY_PIPELINE_LAUNCH(NULL);
}

