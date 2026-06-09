/*************************************************************************
	> File Name: pp_refr.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sun Jun  7 20:30:00 2026
	> Description: Software Rasterizer Refresh Engine driving dirty flush loops.
 ************************************************************************/

#ifndef PP_REFR_H
#define PP_REFR_H

#include "core/pp_disp.h"

#ifndef PP_REFR_INFO
#include <stdio.h>
#define PP_REFR_INFO(fmt,...)  fprintf(stdout, "REFR INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_REFR_ERROR(fmt,...) fprintf(stderr, "REFR ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Execute full refresh timer task loop tracking structural dirty updates */
void pp_display_refr_timer(void);

#ifdef __cplusplus
}
#endif

#endif /* PP_REFR_H */