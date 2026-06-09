/*************************************************************************
	> File Name: pp_canvas.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Sat Jun  6 17:20:00 2026
	> Description: Object-oriented Canvas Manager with unified logging macro styles.
 ************************************************************************/

#ifndef PP_CANVAS_H
#define PP_CANVAS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef PP_CANVAS_INFO
#include <stdio.h>
#define PP_CANVAS_INFO(fmt,...)  fprintf(stdout, "CANVAS INFO %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define PP_CANVAS_ERROR(fmt,...) fprintf(stderr, "CANVAS ERROR %s %d:" fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#endif // PP_CANVAS_INFO

#ifdef __cplusplus
extern "C" {
#endif

/*oOoOoOoOoOoOoOoOoOoO
 * DATA STRUCTURES
 *oOoOoOoOoOoOoOoOoOoO*/

// Canvas entity packed with physical geometry and buffer metadata
typedef struct {
    uint8_t * buffer;       // Raw RGB data stream pointer
    int32_t width;          // Horizontal dimensions in pixels
    int32_t height;         // Vertical dimensions in pixels
    size_t buffer_size;     // Pre-calculated byte budget (width * height * 3)
} pp_canvas_t;

/*oOoOoOoOoOoOoOoOoOoO
 * GLOBAL PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/

pp_canvas_t * pp_canvas_create(int32_t w, int32_t h);
void          pp_canvas_destroy(pp_canvas_t * canvas);
void          pp_canvas_clear(pp_canvas_t * canvas, uint8_t r, uint8_t g, uint8_t b);
void          pp_canvas_export_ppm(const pp_canvas_t * canvas, const char * filename);

#ifdef __cplusplus
}
#endif

#endif /* PP_CANVAS_H */


/*oOoOoOoOoOoOoOoOoOoO
 * IMPLEMENTATION PART
 *oOoOoOoOoOoOoOoOoOoO*/
#if defined(PP_CANVAS_IMPLEMENTATION) && !defined(PP_CANVAS_IMPLEMENTATION_DONE)
#define  PP_CANVAS_IMPLEMENTATION_DONE

#include <stdio.h>
#include <helper/pp_mem.h>

/**
 * @brief Factory Constructor: Allocation of a brand new standalone canvas object
 */
pp_canvas_t * pp_canvas_create(int32_t w, int32_t h)
{
    if (w <= 0 || h <= 0) {
        PP_CANVAS_ERROR("Invalid width or height: w=%d, h=%d", w, h);
        return NULL;
    }

    // 1. Allocate the controlling management structure wrapper safely
    pp_canvas_t * canvas = (pp_canvas_t *)pp_malloc(sizeof(pp_canvas_t));
    if (canvas == NULL) {
        PP_CANVAS_ERROR("Failed to allocate pp_canvas_t structural core wrapper.");
        return NULL;
    }

    canvas->width = w;
    canvas->height = h;
    canvas->buffer_size = (size_t)w * h * 3; // Standard 24-bit RGB pixel payload packing

    // 2. Allocate the deep pixel block memory pool tracking
    canvas->buffer = (uint8_t *)pp_malloc(canvas->buffer_size);
    if (canvas->buffer == NULL) {
        PP_CANVAS_ERROR("OOM! Failed to allocate physical pixel buffer stream: %zu bytes", canvas->buffer_size);
        pp_free(canvas); 
        return NULL;
    }

    // 3. Wash the canvas clean initially to midnight-blue base default coloration
    pp_canvas_clear(canvas, 30, 32, 40);

    return canvas;
}

/**
 * @brief Destructor: Graceful collapse and recycling of all hidden memory nodes
 */
void pp_canvas_destroy(pp_canvas_t * canvas)
{
    if (canvas == NULL) return;

    if (canvas->buffer) {
        pp_free(canvas->buffer);
    }
    
    pp_free(canvas);
}

/**
 * @brief Blit Engine: Fast block paint to wipe canvas to solid uniform coloration
 */
void pp_canvas_clear(pp_canvas_t * canvas, uint8_t r, uint8_t g, uint8_t b)
{
    if (canvas == NULL || canvas->buffer == NULL) {
        PP_CANVAS_ERROR("Attempted to clear a NULL canvas context pointer.");
        return;
    }

    for (size_t i = 0; i < (size_t)canvas->width * canvas->height; i++) {
        canvas->buffer[i * 3 + 0] = r;
        canvas->buffer[i * 3 + 1] = g;
        canvas->buffer[i * 3 + 2] = b;
    }
}

/**
 * @brief IO Exporter: Flush the targeted canvas state natively down to a standard raw P6 PPM graphic
 */
void pp_canvas_export_ppm(const pp_canvas_t * canvas, const char * filename)
{
    if (canvas == NULL || canvas->buffer == NULL || filename == NULL) {
        PP_CANVAS_ERROR("Invalid arguments specified for PPM disk serialization.");
        return;
    }

    FILE *f = fopen(filename, "wb");
    if (!f) {
        PP_CANVAS_ERROR("Failed to create image file disk node target: %s", filename);
        return;
    }

    // Standard raw binary PPM compliance headers
    fprintf(f, "P6\n%d %d\n255\n", canvas->width, canvas->height);
    
    // Laser push the whole packed buffer block directly onto disk
    fwrite(canvas->buffer, canvas->buffer_size, 1, f);
    fclose(f);
    
    PP_CANVAS_INFO("Frame asset securely pushed down to disk path: %s", filename);
}

#endif /* PP_CANVAS_IMPLEMENTATION */
