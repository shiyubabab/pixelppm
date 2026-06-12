#define X86_ARCH
#define PP_MEM_IMPLEMENTATION
#include <helper/pp_mem.h>

#define PP_AREA_IMPLEMENTATION
#include <helper/pp_area.h>

#define PP_CANVAS_IMPLEMENTATION
#include <draw/pp_canvas.h>

#define PP_AREA_DSC_DRAW_IMPLEMENTATION
#include <draw/pp_area_dsc_draw.h>

#define PP_THREAD_IMPLEMENTATION
#include <thread/pp_thread.h>

#include <image/ui_avatar.h>
#include <image/person.h>

#include <core/pp_obj.h>
#include <core/pp_disp.h>
#include <core/pp_refr.h> 
#include <display/pp_show.h> 

#ifndef PP_MAIN_INFO 
#include <stdio.h>
#define PP_MAIN_INFO(fmt,...)  fprintf(stdout, "MAIN INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_MAIN_ERROR(fmt,...) fprintf(stderr, "MAIN ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

int main(void) 
{
    // 1. 初始化物理设备层 (使用 PARTIAL 局部模式，限定单次分幅切片最大 35 行像素)
    if (!pp_disp_init(500, 500, PP_RENDER_MODE_PARTIAL, 35)) {
        PP_MAIN_ERROR("Failed to initialize physical Display layer.");
        return -1;
    }
	pp_disp_t * global_display = pp_disp_get_instance();

    // 调用你添加的颜色宏，一键让全局大墙纸染上深邃色调
    pp_obj_set_bg_color(global_display->root_obj, PP_COLOR_HEX(0x1A1C23));

    // -------------------------------------------------------------------------
    // 2. 轴心静态参照物建立 (用来验证局部刷新时它是否会被 AABB 机制无感拦截)
    // -------------------------------------------------------------------------
    pp_obj_t * static_anchor = pp_obj_create(global_display->root_obj);
    pp_obj_set_pos(static_anchor, 0, 0);
    pp_obj_set_size(static_anchor, 90, 45);
    pp_obj_set_bg_color(static_anchor, PP_COLOR_HEX(0x009688)); // 青绿色

    pp_obj_t * static_anchor_out = pp_obj_create(global_display->root_obj);
    pp_obj_set_pos(static_anchor_out, 50, 50);
	pp_obj_set_image_src(static_anchor_out,&my_avatar);
    pp_obj_set_size(static_anchor_out, my_avatar.width, my_avatar.height);

    pp_obj_t * person = pp_obj_create(global_display->root_obj);
    pp_obj_set_pos(person, 100, 50);
	pp_obj_set_image_src(person,&my_person);
    pp_obj_set_size(person, my_person.width, my_person.height);

    // -------------------------------------------------------------------------
    // 3. 核心运动压测目标：动态多形态按钮 (Moving Target)
    // -------------------------------------------------------------------------
    pp_obj_t * my_button = pp_obj_create(global_display->root_obj);
    pp_obj_set_pos(my_button, 180, 50); // 初始落脚点
    pp_obj_set_size(my_button, 100, 50);
	//pp_obj_set_radius(my_button,5);
    pp_obj_set_bg_color(my_button, PP_COLOR_HEX(0xFFFFFF)); // 初始纯白色

    // =========================================================================
    // 【第一帧：全系统初始化全景铺底】
    // =========================================================================
    PP_MAIN_INFO("====== Executing Frame 1: Core System Initialization ======");
    
    // 全屏报脏以加载首帧底画
    //pp_disp_invalidate_area(&global_display->root_obj->coords);
    
    // 调用已经移位重构的专门刷新时钟函数！
    pp_display_refr_timer();
    
    pp_canvas_export_ppm(global_display->canvas, "iris_gfx_output0.ppm");
    PP_MAIN_INFO("Frame 1 assets outputted to 'iris_gfx_output0.ppm'");

    // =========================================================================
    // 【第二帧：严苛的局部差分异动测试】
    // =========================================================================
    PP_MAIN_INFO("====== Executing Frame 2: Decoupled Partial Refreshing ======");

    // 挪动目标：大跨度飘移到右下角，并一键完成粉红换装
    pp_obj_set_pos(my_button, 250, 240);
    pp_obj_set_bg_color(my_button, PP_COLOR_HEX(0xE91E63));
    pp_obj_set_bg_opa(my_button, 100);
    
    // 【再】将它的【新坐标区域】标记为脏！

    // 运筹帷幄：大步流星调用专门的独立刷新引擎进行局部消灭战！
    pp_display_refr_timer();

    pp_canvas_export_ppm(global_display->canvas, "iris_gfx_output1.ppm");
    PP_MAIN_INFO("Frame 2 assets outputted to 'iris_gfx_output1.ppm'");

	pp_show_engine_start();
    // -------------------------------------------------------------------------
    // 4. 清理物理显示驱动，完美退场
    // -------------------------------------------------------------------------
    pp_disp_deinit();
    PP_MAIN_INFO("Pipeline shutdown cleanly. Test completely passed.");

    
    return 0;
}
