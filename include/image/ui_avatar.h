/*************************************************************************
	> File Name: ui_avatar.h
	> Author: mlxh
	> Mail: mlxh_gto@163.com 
	> Created Time: Mon Jun  8 20:30:00 2026
	> Description: Procedurally generated 80x80 gradient test image token.
 ************************************************************************/

#ifndef PP_IMG_DATA_MY_AVATAR_H
#define PP_IMG_DATA_MY_AVATAR_H

#include "draw/pp_area_dsc_draw.h"

// ⚡ 核心算法手工贴图生成：这里我们在编译期无法用 for 循环，
// 所以我用宏在编译期静态展开一个 80x80 的双向颜色渐变阵列！
// 边缘是红蓝渐变，越往中心走绿光越强，极其适合肉眼Debug观察百叶窗切片拼装是否有错位。

#define R_GRAD(x, y) ((uint8_t)((x) * 255 / 79))
#define B_GRAD(x, y) ((uint8_t)((y) * 255 / 79))
#define G_CENTER(x, y) ((uint8_t)(((x) > 25 && (x) < 55 && (y) > 25 && (y) < 55) ? 255 : 0))

// 1. 灌入 6400 个绝对合法的原始只读像素实体流
// 为了不让 6400 行代码撑爆你的编辑器，我们用宏的递推或直接铺设：
static const pp_color_t my_avatar_pixel_map[80 * 80] = {
#define ROW_80(y) \
    {R_GRAD(0,y),G_CENTER(0,y),B_GRAD(0,y)}, {R_GRAD(1,y),G_CENTER(1,y),B_GRAD(1,y)}, \
    {R_GRAD(2,y),G_CENTER(2,y),B_GRAD(2,y)}, {R_GRAD(3,y),G_CENTER(3,y),B_GRAD(3,y)}, \
    {R_GRAD(4,y),G_CENTER(4,y),B_GRAD(4,y)}, {R_GRAD(5,y),G_CENTER(5,y),B_GRAD(5,y)}, \
    {R_GRAD(6,y),G_CENTER(6,y),B_GRAD(6,y)}, {R_GRAD(7,y),G_CENTER(7,y),B_GRAD(7,y)}, \
    {R_GRAD(8,y),G_CENTER(8,y),B_GRAD(8,y)}, {R_GRAD(9,y),G_CENTER(9,y),B_GRAD(9,y)}, \
    {R_GRAD(10,y),G_CENTER(10,y),B_GRAD(10,y)}, {R_GRAD(11,y),G_CENTER(11,y),B_GRAD(11,y)}, \
    {R_GRAD(12,y),G_CENTER(12,y),B_GRAD(12,y)}, {R_GRAD(13,y),G_CENTER(13,y),B_GRAD(13,y)}, \
    {R_GRAD(14,y),G_CENTER(14,y),B_GRAD(14,y)}, {R_GRAD(15,y),G_CENTER(15,y),B_GRAD(15,y)}, \
    {R_GRAD(16,y),G_CENTER(16,y),B_GRAD(16,y)}, {R_GRAD(17,y),G_CENTER(17,y),B_GRAD(17,y)}, \
    {R_GRAD(18,y),G_CENTER(18,y),B_GRAD(18,y)}, {R_GRAD(19,y),G_CENTER(19,y),B_GRAD(19,y)}, \
    {R_GRAD(20,y),G_CENTER(20,y),B_GRAD(20,y)}, {R_GRAD(21,y),G_CENTER(21,y),B_GRAD(21,y)}, \
    {R_GRAD(22,y),G_CENTER(22,y),B_GRAD(22,y)}, {R_GRAD(23,y),G_CENTER(23,y),B_GRAD(23,y)}, \
    {R_GRAD(24,y),G_CENTER(24,y),B_GRAD(24,y)}, {R_GRAD(25,y),G_CENTER(25,y),B_GRAD(25,y)}, \
    {R_GRAD(26,y),G_CENTER(26,y),B_GRAD(26,y)}, {R_GRAD(27,y),G_CENTER(27,y),B_GRAD(27,y)}, \
    {R_GRAD(28,y),G_CENTER(28,y),B_GRAD(28,y)}, {R_GRAD(29,y),G_CENTER(29,y),B_GRAD(29,y)}, \
    {R_GRAD(30,y),G_CENTER(30,y),B_GRAD(30,y)}, {R_GRAD(31,y),G_CENTER(31,y),B_GRAD(31,y)}, \
    {R_GRAD(32,y),G_CENTER(32,y),B_GRAD(32,y)}, {R_GRAD(33,y),G_CENTER(33,y),B_GRAD(33,y)}, \
    {R_GRAD(34,y),G_CENTER(34,y),B_GRAD(34,y)}, {R_GRAD(35,y),G_CENTER(35,y),B_GRAD(35,y)}, \
    {R_GRAD(36,y),G_CENTER(36,y),B_GRAD(36,y)}, {R_GRAD(37,y),G_CENTER(37,y),B_GRAD(37,y)}, \
    {R_GRAD(38,y),G_CENTER(38,y),B_GRAD(38,y)}, {R_GRAD(39,y),G_CENTER(39,y),B_GRAD(39,y)}, \
    {R_GRAD(40,y),G_CENTER(40,y),B_GRAD(40,y)}, {R_GRAD(41,y),G_CENTER(41,y),B_GRAD(41,y)}, \
    {R_GRAD(42,y),G_CENTER(42,y),B_GRAD(42,y)}, {R_GRAD(43,y),G_CENTER(43,y),B_GRAD(43,y)}, \
    {R_GRAD(44,y),G_CENTER(44,y),B_GRAD(44,y)}, {R_GRAD(45,y),G_CENTER(45,y),B_GRAD(45,y)}, \
    {R_GRAD(46,y),G_CENTER(46,y),B_GRAD(46,y)}, {R_GRAD(47,y),G_CENTER(47,y),B_GRAD(47,y)}, \
    {R_GRAD(48,y),G_CENTER(48,y),B_GRAD(48,y)}, {R_GRAD(49,y),G_CENTER(49,y),B_GRAD(49,y)}, \
    {R_GRAD(50,y),G_CENTER(50,y),B_GRAD(50,y)}, {R_GRAD(51,y),G_CENTER(51,y),B_GRAD(51,y)}, \
    {R_GRAD(52,y),G_CENTER(52,y),B_GRAD(52,y)}, {R_GRAD(53,y),G_CENTER(53,y),B_GRAD(53,y)}, \
    {R_GRAD(54,y),G_CENTER(54,y),B_GRAD(54,y)}, {R_GRAD(55,y),G_CENTER(55,y),B_GRAD(55,y)}, \
    {R_GRAD(56,y),G_CENTER(56,y),B_GRAD(56,y)}, {R_GRAD(57,y),G_CENTER(57,y),B_GRAD(57,y)}, \
    {R_GRAD(58,y),G_CENTER(58,y),B_GRAD(58,y)}, {R_GRAD(59,y),G_CENTER(59,y),B_GRAD(59,y)}, \
    {R_GRAD(60,y),G_CENTER(60,y),B_GRAD(60,y)}, {R_GRAD(61,y),G_CENTER(61,y),B_GRAD(61,y)}, \
    {R_GRAD(62,y),G_CENTER(62,y),B_GRAD(62,y)}, {R_GRAD(63,y),G_CENTER(63,y),B_GRAD(63,y)}, \
    {R_GRAD(64,y),G_CENTER(64,y),B_GRAD(64,y)}, {R_GRAD(65,y),G_CENTER(65,y),B_GRAD(65,y)}, \
    {R_GRAD(66,y),G_CENTER(66,y),B_GRAD(66,y)}, {R_GRAD(67,y),G_CENTER(67,y),B_GRAD(67,y)}, \
    {R_GRAD(68,y),G_CENTER(68,y),B_GRAD(68,y)}, {R_GRAD(69,y),G_CENTER(69,y),B_GRAD(69,y)}, \
    {R_GRAD(70,y),G_CENTER(70,y),B_GRAD(70,y)}, {R_GRAD(71,y),G_CENTER(71,y),B_GRAD(71,y)}, \
    {R_GRAD(72,y),G_CENTER(72,y),B_GRAD(72,y)}, {R_GRAD(73,y),G_CENTER(73,y),B_GRAD(73,y)}, \
    {R_GRAD(74,y),G_CENTER(74,y),B_GRAD(74,y)}, {R_GRAD(75,y),G_CENTER(75,y),B_GRAD(75,y)}, \
    {R_GRAD(76,y),G_CENTER(76,y),B_GRAD(76,y)}, {R_GRAD(77,y),G_CENTER(77,y),B_GRAD(77,y)}, \
    {R_GRAD(78,y),G_CENTER(78,y),B_GRAD(78,y)}, {R_GRAD(79,y),G_CENTER(79,y),B_GRAD(79,y)}

    // 静态展开 80 行数据，全部常驻 Flash！
    ROW_80(0),  ROW_80(1),  ROW_80(2),  ROW_80(3),  ROW_80(4),  ROW_80(5),  ROW_80(6),  ROW_80(7),
    ROW_80(8),  ROW_80(9),  ROW_80(10), ROW_80(11), ROW_80(12), ROW_80(13), ROW_80(14), ROW_80(15),
    ROW_80(16), ROW_80(17), ROW_80(18), ROW_80(19), ROW_80(20), ROW_80(21), ROW_80(22), ROW_80(23),
    ROW_80(24), ROW_80(25), ROW_80(26), ROW_80(27), ROW_80(28), ROW_80(29), ROW_80(30), ROW_80(31),
    ROW_80(32), ROW_80(33), ROW_80(34), ROW_80(35), ROW_80(36), ROW_80(37), ROW_80(38), ROW_80(39),
    ROW_80(40), ROW_80(41), ROW_80(42), ROW_80(43), ROW_80(44), ROW_80(45), ROW_80(46), ROW_80(47),
    ROW_80(48), ROW_80(49), ROW_80(50), ROW_80(51), ROW_80(52), ROW_80(53), ROW_80(54), ROW_80(55),
    ROW_80(56), ROW_80(57), ROW_80(58), ROW_80(59), ROW_80(60), ROW_80(61), ROW_80(62), ROW_80(63),
    ROW_80(64), ROW_80(65), ROW_80(66), ROW_80(67), ROW_80(68), ROW_80(69), ROW_80(70), ROW_80(71),
    ROW_80(72), ROW_80(73), ROW_80(74), ROW_80(75), ROW_80(76), ROW_80(77), ROW_80(78), ROW_80(79)
};

// 2. 自动收拢、组装成高级只读资产描述控制块
static const pp_image_t my_avatar = {
    .pixel_map = my_avatar_pixel_map,
    .width = 80,
    .height = 80
};

#endif // PP_IMG_DATA_MY_AVATAR_H
