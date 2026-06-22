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

#if defined(PP_UV_LOOP_IMPLEMENTATION) && !defined(PP_UV_LOOP_IMPLEMENTATION_DONE)
#define PP_UV_LOOP_IMPLEMENTATION_DONE

#include <uv.h>
#include "core/pp_refr.h"
#include "image/ui_avatar.h"

uv_timer_t timer_handle;

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

	pp_obj_t * oo1 = pp_obj_create(global_display->root_obj);
	pp_obj_set_pos(oo1,250,250);
	pp_obj_set_image_src(oo1,&my_avatar);
	pp_obj_set_size(oo1,my_avatar.width,my_avatar.height);

}

static bool can_render= false;

static void pp_render_pipeline_cb(uv_timer_t *handle)
{
	pp_disp_t * global_display = pp_disp_get_instance();
	PP_ASSERT(handle && global_display);
	pp_input_process();
	pp_draw_frame();

	if(can_render){
		pp_display_refr_timer();
		uv_mutex_lock(&global_display->lock);
		global_display->render_complete = true;
		uv_mutex_unlock(&global_display->lock);
		uv_async_send(&global_display->async_to_display);
	}

	uv_timer_start(&timer_handle,pp_render_pipeline_cb,0,0);
}

void on_vsync_signal_from_display(uv_async_t * handle){
	pp_disp_t * global_display = pp_disp_get_instance();
	PP_ASSERT(handle && global_display);

	can_render= false;
	uv_mutex_lock(&global_display->lock);
	if(global_display->v_sync_ready && !global_display->render_complete){
		can_render = true;
		global_display->v_sync_ready = false;
	}
	uv_mutex_unlock(&global_display->lock);
}

void pp_loop_start(void)
{
	pp_disp_t * global_display = pp_disp_get_instance();
	if(!global_display){
		PP_UV_ERROR("Fail to get global display");
		return;
	}


	uv_loop_t* render_loop = uv_default_loop();
	global_display->render_loop = render_loop;
	uv_async_init(render_loop,&global_display->async_to_ui,on_vsync_signal_from_display);

	uv_timer_init(render_loop,&timer_handle);

	uv_timer_start(&timer_handle,pp_render_pipeline_cb,0,0);

	uv_run(render_loop, UV_RUN_DEFAULT);

	uv_loop_close(render_loop);
}

#endif//  PP_UV_LOOP_IMPLEMENTATION
