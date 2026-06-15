#include "common.h"


int main(void) 
{
    if (!pp_disp_init(500, 500, PP_RENDER_MODE_PARTIAL, 35)) {
        PP_MAIN_ERROR("Failed to initialize physical Display layer.");
        return -1;
    }

	// Init thread pool with 10 thread and 100 capacity of task ringbuffer.
	if(!pp_thread_pool_init(10,128)){
		PP_MAIN_ERROR("Fail to init thread pool");
		return -1;
	}

	// display thread.
	pp_show_engine_start();

	// ui main thread.
	pp_loop_start();

    pp_disp_deinit();
    PP_MAIN_INFO("Pipeline shutdown cleanly. Test completely passed.");
    return 0;
}
