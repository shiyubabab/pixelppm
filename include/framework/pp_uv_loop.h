/*************************************************************************
	> File Name: pp_uv_loop.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sun 14 Jun 2026 06:25:33 PM CST
 ************************************************************************/

#ifndef PP_UV_LOOP_H
#define PP_UV_LOOP_H

#ifndef PP_UV_INFO
#define PP_UV_INFO(fmt,...) fprintf(stdout,"UV INFO %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#define PP_UV_ERROR(fmt,...) fprintf(stderr,"UV ERROR %s %d:"fmt"\n",__func__,__LINE__,##__VA_ARGS__);
#endif

void pp_loop_start(void);

#endif // PP_UV_LOOP_H

#ifdef PP_UV_LOOP_IMPLEMENTATION
#include <uv.h>
#include "core/pp_refr.h"
#include "image/ui_avatar.h"

static void pp_input_process(void)
{
	PP_UV_INFO("TODO pp_input_process");
}

static void pp_draw_frame(void)
{
	PP_UV_INFO("TODO pp_draw_frame");

	pp_disp_t * global_display = pp_disp_get_instance();
	pp_obj_set_bg_color(global_display->root_obj,PP_COLOR_HEX(0x1A1C23));

	pp_obj_t * oo = pp_obj_create(global_display->root_obj);
	pp_obj_set_pos(oo,50,50);
	pp_obj_set_image_src(oo,&my_avatar);
	pp_obj_set_size(oo,my_avatar.width,my_avatar.height);

}

static void pp_render_pipeline_cb(uv_timer_t *handle)
{
	PP_ASSERT(handle);
	pp_input_process();
	pp_draw_frame();
	pp_display_refr_timer();
}

void pp_loop_start(void)
{
	uv_loop_t *loop = uv_default_loop();
	uv_timer_t timer_handle;

	uv_timer_init(loop,&timer_handle);

	uv_timer_start(&timer_handle,pp_render_pipeline_cb,0,10);

	uv_run(loop, UV_RUN_DEFAULT);

	uv_loop_close(loop);
}

#endif//  PP_UV_LOOP_IMPLEMENTATION
