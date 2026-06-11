/*************************************************************************
	> File Name: nob.c
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Fri May 22 15:28:11 2026
	> Description: Automated NOB build pipeline tracking structural updates.
 ************************************************************************/

#define NOB_IMPLEMENTATION
#include "nob.h"

#define INCLUDE_DIR    "./include/"
#define SRC_DIR        "./src/"
#define CORE_SRC_DIR   SRC_DIR"core/"

/**
 * @brief Initialize base compiler configurations
 */
void cc(Varray *cmd)
{
    nob_cmd_append(cmd, "gcc");
    nob_cmd_append(cmd, "-Wall", "-Wextra");
    nob_cmd_append(cmd, "-D_GNU_SOURCE");
    nob_cmd_append(cmd, "-ggdb");
    nob_cmd_append(cmd, "-Wno-unused-function");
}

/**
 * @brief Inject deep multi-tier include search paths
 */
void cc_include(Varray *cmd)
{
    // Enable seamless #include <core/pp_obj.h> or #include <draw/pp_canvas.h>
    nob_cmd_append(cmd, "-I" INCLUDE_DIR);
    nob_cmd_append(cmd, "-I.");
}

/**
 * @brief Link critical runtime and hardware library backends
 */
void cc_lib(Varray *cmd)
{
    nob_cmd_append(cmd, "-L/usr/local/lib");
    nob_cmd_append(cmd, "-Wl,-rpath,/usr/local/lib");

    nob_cmd_append(cmd, "-luv");
    nob_cmd_append(cmd, "-lm", "-ldl", "-lpthread");
}

/**
 * @brief Main pipeline compiler matching the new structural source files
 */
void cc_main(Varray *cmd)
{
    cc(cmd);
    cc_include(cmd);

    // 1. Append the main execution application entry point
    nob_cmd_append(cmd, "main.c");

    // 2. HARDCORE UPDATE: Append the real .c implementations hidden inside src/core/
    nob_cmd_append(cmd, CORE_SRC_DIR"pp_obj.c");
    nob_cmd_append(cmd, CORE_SRC_DIR"pp_obj_class.c");
    nob_cmd_append(cmd, CORE_SRC_DIR"pp_disp.c");
    nob_cmd_append(cmd, CORE_SRC_DIR"pp_refr.c");

    // 3. Setup the absolute executable artifact output naming target
    nob_cmd_append(cmd, "-o", "go");

    cc_lib(cmd);

    // Trigger the atomic NOB engine compilation shell executor
    NOB_BUILD_PROJECT(cmd);
}

void dump_cmd(Varray cmd, char *name)
{
    printf("%s capacity[%zu] count[%zu] = [", name, cmd.capacity, cmd.count);
    for(int i = 0; i < cmd.count; ++i)
        printf("%s ", cmd.data[i]);
    printf("]\n");
}

#define CC_DUMP_CMD(cmd) dump_cmd(cmd, #cmd)

int main(void)
{
    Varray cmd = {0};

    // Execute the main structural build tracking core changes
    cc_main(&cmd);
    CC_DUMP_CMD(cmd);

    nob_cmd_free(&cmd);
    return 0;
}
