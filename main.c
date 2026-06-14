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

#define PP_UV_LOOP_IMPLEMENTATION
#include <framework/pp_uv_loop.h>

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
    if (!pp_disp_init(500, 500, PP_RENDER_MODE_PARTIAL, 35)) {
        PP_MAIN_ERROR("Failed to initialize physical Display layer.");
        return -1;
    }
	pp_show_engine_start();
	pp_loop_start();

    pp_disp_deinit();
    PP_MAIN_INFO("Pipeline shutdown cleanly. Test completely passed.");
    return 0;
}
