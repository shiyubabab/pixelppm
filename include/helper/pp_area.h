/*************************************************************************
	> File Name: pp_area.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Thu Jun  4 19:20:00 2026
	> Description: Ultra-performance STB-style geometry engine aligned with LVGL.
 ************************************************************************/

#ifndef PP_AREA_H
#define PP_AREA_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*oOoOoOoOoOoOoOoOoOoO
 * DATA STRUCTURES
 *oOoOoOoOoOoOoOoOoOoO*/

typedef struct {
    int32_t x;
    int32_t y;
} pp_point_t;

typedef struct {
    int32_t x1; // 左上角 X
    int32_t y1; // 左上角 Y
    int32_t x2; // 右下角 X
    int32_t y2; // 右下角 Y
} pp_area_t;

/*oOoOoOoOoOoOoOoOoOoO
 * HEADER PART: Inlines
 *oOoOoOoOoOoOoOoOoOoO*/

static inline int32_t pp_area_get_width(const pp_area_t * area) {
    return area ? (area->x2 - area->x1 + 1) : 0;
}

static inline int32_t pp_area_get_height(const pp_area_t * area) {
    return area ? (area->y2 - area->y1 + 1) : 0;
}

static inline uint32_t pp_area_get_size(const pp_area_t * area) {
    return area ? (uint32_t)pp_area_get_width(area) * pp_area_get_height(area) : 0;
}

static inline void pp_area_set(pp_area_t * area, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    if (area) { area->x1 = x1; area->y1 = y1; area->x2 = x2; area->y2 = y2; }
}

static inline void pp_area_move(pp_area_t * area, int32_t x_ofs, int32_t y_ofs) {
    if (area) { area->x1 += x_ofs; area->x2 += x_ofs; area->y1 += y_ofs; area->y2 += y_ofs; }
}

/*oOoOoOoOoOoOoOoOoOoO
 * GLOBAL PROTOTYPES
 *oOoOoOoOoOoOoOoOoOoO*/

bool pp_area_intersect(pp_area_t * res, const pp_area_t * a1, const pp_area_t * a2);
void pp_area_join(pp_area_t * res, const pp_area_t * a1, const pp_area_t * a2);
bool pp_area_is_on(const pp_area_t * a1, const pp_area_t * a2);
bool pp_area_is_equal(const pp_area_t * a, const pp_area_t * b);
bool pp_area_is_point_on(const pp_area_t * area, const pp_point_t * p, int32_t radius);
bool pp_area_is_in(const pp_area_t * child, const pp_area_t * parent, int32_t radius);
bool pp_area_is_out(const pp_area_t * child, const pp_area_t * parent, int32_t radius);

/* 【硬核加餐】对齐 LVGL 的高级面积切割 Diff 算法与顶点旋转矩阵 */
int8_t pp_area_diff(pp_area_t res_array[], const pp_area_t * a1, const pp_area_t * a2);
void pp_point_array_transform(pp_point_t * points, size_t count, int32_t angle, int32_t scale_x, int32_t scale_y, const pp_point_t * pivot);

#ifdef __cplusplus
}
#endif

#endif /* PP_AREA_H */

/*oOoOoOoOoOoOoOoOoOoO
 * IMPLEMENTATION PART
 *oOoOoOoOoOoOoOoOoOoO*/
#if defined(PP_AREA_IMPLEMENTATION) && !defined(PP_AREA_IMPLEMENTATION_DOWN)
#define PP_AREA_IMPLEMENTATION_DOWN

#ifndef PP_MIN
#define PP_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef PP_MAX
#define PP_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * @brief 核心裁剪：计算相交区域
 */
bool pp_area_intersect(pp_area_t * res, const pp_area_t * a1, const pp_area_t * a2)
{
    if(!res || !a1 || !a2) return false;
    res->x1 = PP_MAX(a1->x1, a2->x1);
    res->y1 = PP_MAX(a1->y1, a2->y1);
    res->x2 = PP_MIN(a1->x2, a2->x2);
    res->y2 = PP_MIN(a1->y2, a2->y2);

    if((res->x1 > res->x2) || (res->y1 > res->y2)) {
        return false; // 无交集
    }
    return true;
}

/**
 * @brief 核心融合：计算并集超级包裹区
 */
void pp_area_join(pp_area_t * res, const pp_area_t * a1, const pp_area_t * a2)
{
    if(!res || !a1 || !a2) return;
    res->x1 = PP_MIN(a1->x1, a2->x1);
    res->y1 = PP_MIN(a1->y1, a2->y1);
    res->x2 = PP_MAX(a1->x2, a2->x2);
    res->y2 = PP_MAX(a1->y2, a2->y2);
}

/**
 * @brief 快速交叉碰撞检查 这个函数比计算相交区域函数要快，其没有写操作，只有读操作。
 */
bool pp_area_is_on(const pp_area_t * a1, const pp_area_t * a2)
{
    if(!a1 || !a2) return false;
    return (a1->x1 <= a2->x2 && a1->x2 >= a2->x1 && a1->y1 <= a2->y2 && a1->y2 >= a2->y1);
}

bool pp_area_is_equal(const pp_area_t * a, const pp_area_t * b)
{
    return (a && b) ? (a->x1 == b->x1 && a->x2 == b->x2 && a->y1 == b->y1 && a->y2 == b->y2) : false;
}

/* 内部圆角辅助判定 */
static bool pp_point_within_circle(const pp_area_t * area, const pp_point_t * p) {
    int32_t r = (area->x2 - area->x1) / 2;
    int32_t cx = area->x1 + r, cy = area->y1 + r;
    int32_t px = p->x - cx, py = p->y - cy;
    return (uint32_t)(px * px + py * py) <= (uint32_t)(r * r);
}

/**
 * @brief 工业级判定：点是否在（圆角）矩形内
 */
bool pp_area_is_point_on(const pp_area_t * area, const pp_point_t * p, int32_t radius)
{
    if(!area || !p) return false;
    // 1. 先过 AABB 大关
    if(p->x < area->x1 || p->x > area->x2 || p->y < area->y1 || p->y > area->y2) return false;
    if(radius <= 0) return true;

    int32_t max_r = PP_MIN(pp_area_get_width(area) / 2, pp_area_get_height(area) / 2);
    if(radius > max_r) radius = max_r;

    // 2. 判定是否踩在死角直角扇区（左上、左下、右下、右上），如果是，套用圆形方程二次精筛
    pp_area_t corner;
    corner.x1 = area->x1; corner.x2 = area->x1 + radius;
    corner.y1 = area->y1; corner.y2 = area->y1 + radius;
    if(p->x <= corner.x2 && p->y <= corner.y2) { // 左上角
        corner.x2 += radius; corner.y2 += radius;
        return pp_point_within_circle(&corner, p);
    }
    corner.y1 = area->y2 - radius; corner.y2 = area->y2;
    if(p->x <= corner.x2 && p->y >= corner.y1) { // 左下角
        corner.x2 += radius; corner.y1 -= radius;
        return pp_point_within_circle(&corner, p);
    }
    corner.x1 = area->x2 - radius; corner.x2 = area->x2;
    if(p->x >= corner.x1 && p->y >= corner.y1) { // 右下角
        corner.x1 -= radius; corner.y1 -= radius;
        return pp_point_within_circle(&corner, p);
    }
    corner.y1 = area->y1; corner.y2 = area->y1 + radius;
    if(p->x >= corner.x1 && p->y <= corner.y2) { // 右上角
        corner.x1 -= radius; corner.y2 += radius;
        return pp_point_within_circle(&corner, p);
    }
    return true;
}

bool pp_area_is_in(const pp_area_t * child, const pp_area_t * parent, int32_t radius)
{
    if(!child || !parent) return false;
    if(child->x1 < parent->x1 || child->y1 < parent->y1 || child->x2 > parent->x2 || child->y2 > parent->y2) return false;
    if(radius == 0) return true;
    pp_point_t p;
    p.x = child->x1; p.y = child->y1; if(!pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x2; p.y = child->y1; if(!pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x1; p.y = child->y2; if(!pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x2; p.y = child->y2; if(!pp_area_is_point_on(parent, &p, radius)) return false;
    return true;
}

bool pp_area_is_out(const pp_area_t * child, const pp_area_t * parent, int32_t radius)
{
    if(!child || !parent) return true;
    if(child->x2 < parent->x1 || child->y2 < parent->y1 || child->x1 > parent->x2 || child->y1 > parent->y2) return true;
    if(radius == 0) return false;
    pp_point_t p;
    p.x = child->x1; p.y = child->y1; if(pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x2; p.y = child->y1; if(pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x1; p.y = child->y2; if(pp_area_is_point_on(parent, &p, radius)) return false;
    p.x = child->x2; p.y = child->y2; if(pp_area_is_point_on(parent, &p, radius)) return false;
    return true;
}

/**
 * @brief 极客算法：脏区无损差集切割 (对齐 lv_area_diff)
 * @return 裁剪分离出来的有效碎块矩形数量（最大4个），-1代表无交集，0代表全覆盖
 */
int8_t pp_area_diff(pp_area_t res_array[], const pp_area_t * a1, const pp_area_t * a2)
{
    if(!pp_area_is_on(a1, a2)) return -1;
    if(pp_area_is_in(a1, a2, 0)) return 0;

    int8_t count = 0;
    pp_area_t chunk;
    int32_t a1_w = pp_area_get_width(a1) - 1;
    int32_t a1_h = pp_area_get_height(a1) - 1;

    /* 1. 切割上方碎块 */
    int32_t th = a2->y1 - a1->y1;
    if(th > 0) {
        pp_area_set(&chunk, a1->x1, a1->y1, a1->x2, a1->y1 + th - 1);
        res_array[count++] = chunk;
    }
    /* 2. 切割下方碎块 */
    int32_t bh = a1_h - (a2->y2 - a1->y1);
    if(bh > 0 && a2->y2 < a1->y2) {
        pp_area_set(&chunk, a1->x1, a2->y2 + 1, a1->x2, a2->y2 + bh);
        res_array[count++] = chunk;
    }
    /* 3. 约束中段侧高 */
    int32_t y1 = PP_MAX(a2->y1, a1->y1);
    int32_t y2 = PP_MIN(a2->y2, a1->y2);
    int32_t sh = y2 - y1;

    /* 4. 切割左侧碎块 */
    int32_t lw = a2->x1 - a1->x1;
    if(lw > 0 && sh >= 0) {
        pp_area_set(&chunk, a1->x1, y1, a1->x1 + lw - 1, y1 + sh);
        res_array[count++] = chunk;
    }
    /* 5. 切割右侧碎块 */
    int32_t rw = a1_w - (a2->x2 - a1->x1);
    if(rw > 0 && sh >= 0) {
        pp_area_set(&chunk, a2->x2 + 1, y1, a2->x2 + rw, y1 + sh);
        res_array[count++] = chunk;
    }
    return count;
}

/**
 * @brief 白嫖硬件浮点的定点数顶点变换矩阵（支持指针围绕中心轴多点顺畅旋转与缩放）
 * @param angle 0~3600 代表 0.0 ~ 360.0 度
 */
void pp_point_transform(pp_point_t * point, int32_t angle, int32_t scale_x, int32_t scale_y, const pp_point_t * pivot)
{
    if(angle == 0 && scale_x == 256 && scale_y == 256) return;
    
    // 粗暴平移回仿射变换原点 (0,0)
    int32_t x = point->x - pivot->x;
    int32_t y = point->y - pivot->y;

    // 这里为了单文件闭环，我们采用微型定点数三角公式（实际可接入快速查表法）
    // 模拟纯硬件查表位移移位（用 1024 替代浮点数，对齐 LVGL 的 TRIGO_SHIFT）
    double rad = (angle / 10.0) * 3.14159265 / 180.0;
    int32_t cosma = (int32_t)(cos(rad) * 1024);
    int32_t sinma = (int32_t)(sin(rad) * 1024);

    if(angle == 0) {
        point->x = ((x * scale_x) >> 8) + pivot->x;
        point->y = ((y * scale_y) >> 8) + pivot->y;
    } else {
        x = (x * scale_x) >> 8;
        y = (y * scale_y) >> 8;
        point->x = ((cosma * x - sinma * y) >> 10) + pivot->x;
        point->y = ((sinma * x + cosma * y) >> 10) + pivot->y;
    }
}

void pp_point_array_transform(pp_point_t * points, size_t count, int32_t angle, int32_t scale_x, int32_t scale_y, const pp_point_t * pivot) {
    for(size_t i = 0; i < count; i++) {
        pp_point_transform(&points[i], angle, scale_x, scale_y, pivot);
    }
}

#endif /* PP_AREA_IMPLEMENTATION */
