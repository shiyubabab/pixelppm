#!/usr/bin/env python3
# iris_chroma_converter.py
import os
import sys
from PIL import Image

def generate_clean_iris_header(img_path, output_h_path, var_name="my_avatar"):
    if not os.path.exists(img_path):
        print(f"Error: Cannot find input image at '{img_path}'")
        return

    # 1. 强转为 RGB 物理色彩空间进行像素精细比对
    img = Image.open(img_path).convert('RGB')
    width, height = img.size
    guard_name = f"PP_IMG_DATA_{var_name.upper()}_H"

    with open(output_h_path, "w", encoding="utf-8") as f:
        # 写入文件保护哨和依赖头
        f.write("/*************************************************************************\n")
        f.write(f"\t> File Name: {os.path.basename(output_h_path)}\n")
        f.write("\t> Description: Pure Character Extraction (Color Key Purged for Checkerboard).\n")
        f.write(" ************************************************************************/\n\n")
        f.write(f"#ifndef {guard_name}\n")
        f.write(f"#define {guard_name}\n\n")
        f.write('#include "draw/pp_area_dsc_draw.h"\n\n')

        # 2. 动态生成符合当前图片宽度的行扫描宏定义
        f.write(f"// ⚡ 完美的行扫描像素映射宏，自动熔断剔除灰白棋盘格与压缩噪点\n")
        f.write(f"#define ROW_{width}_RGBA(y) \\\n")
        
        lines = []
        for y in range(height):
            macro_line_pixels = []
            for x in range(width):
                r, g, b = img.getpixel((x, y))
                
                # ⚡⚡【核心硬核滤镜算法：色差饱和度与亮度双重绞杀】
                # 条件一：R, G, B 三分量均大于 180 (覆盖了所有白块、浅灰块、中灰压缩毛边)
                # 条件二：彼此之间的绝对差值小于 15 (证明它没有任何彩色饱和度，纯属无彩色的灰白背景)
                if (r > 180 and g > 180 and b > 180) and (abs(r - g) < 15 and abs(g - b) < 15):
                    alpha = 0
                    r_out, g_out, b_out = 0, 0, 0  # 色彩清零，防止半透明插值时产生脏边渗漏
                else:
                    alpha = 255
                    r_out, g_out, b_out = r, g, b  # 真正的人物品质，饱满保留
                    
                macro_line_pixels.append(f"{{ {r_out}, {g_out}, {b_out}, {alpha} }}")
                
            line_str = ", ".join(macro_line_pixels)
            if y < height - 1:
                lines.append(f"    {line_str}, \\")
            else:
                lines.append(f"    {line_str}")
                
        f.write("\n".join(lines) + "\n\n")

        # 3. 映射常驻 Flash 的静态 RGBA 数组
        f.write(f"// 1. 物理像素实体流（已完全剥离黑白格子底）\n")
        f.write(f"static const pp_color_t {var_name}_pixel_map[{width * height}] = {{\n")
        f.write(f"    ROW_{width}_RGBA(0)\n")
        f.write("};\n\n")

        # 4. 高级只读描述控制块
        f.write(f"// 2. 资产描述控制符\n")
        f.write(f"static const pp_image_t {var_name} = {{\n")
        f.write(f"    .pixel_map = {var_name}_pixel_map,\n")
        f.write(f"    .width = {width},\n")
        f.write(f"    .height = {height}\n")
        f.write("};\n\n")
        
        f.write(f"#endif // {guard_name}\n")

    print(f"Success! Pure asset generated at '{output_h_path}' [{width}x{height} px]")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 iris_chroma_converter.py <input_png> <output_h> [variable_name]")
    else:
        v_name = sys.argv[3] if len(sys.argv) > 3 else "my_avatar"
        generate_clean_iris_header(sys.argv[1], sys.argv[2], v_name)