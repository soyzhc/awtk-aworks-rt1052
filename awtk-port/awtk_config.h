﻿
/**
 * File:   awtk_config.h
 * Author: AWTK Develop Team
 * Brief:  config
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-09-12 Li XianJing <xianjimli@hotmail.com> created
 */

#ifndef AWTK_CONFIG_H
#define AWTK_CONFIG_H

/**
 * 嵌入式系统有自己的main函数时，请定义本宏。
 *
 */
#define USE_GUI_MAIN 1

/**
 * 如果支持png/jpeg图片，请定义本宏
 *
 */
#define WITH_STB_IMAGE 1

/**
 * 如果支持Truetype字体，请定义本宏
 *
 */
#define WITH_STB_FONT 1

/**
 * 如果定义本宏，使用标准的UNICODE换行算法，除非资源极为有限，请定义本宏。
 *
 */
#define WITH_UNICODE_BREAK 1

/**
 * 如果定义本宏，将图片解码成BGRA8888格式，否则解码成RGBA8888的格式。
 *
 */
#define WITH_BITMAP_BGRA 1

/**
 * 如果定义本宏，将不透明的PNG图片解码成BGR565格式，建议定义。
 *
 */
#define WITH_BITMAP_BGR565 1

/**
 * 如果有优化版本的memcpy函数，请定义本宏
 *
 */
#define HAS_FAST_MEMCPY 1

/**
 * 如果系统有标准的malloc函数，请定义本宏
 *
 */
#define HAS_STD_MALLOC 1

/**
 * 如果启用NXP PXP硬件加速，请定义本宏
 * 
 */
#define WITH_PXP_G2D 1

/**
 * 如果启用三缓冲模式，请定义本宏，否则默认使用双缓冲模式
 *
 * 注意：如果使用 tk_set_lcd_orientation 旋转屏幕，则应该注释该行，
 *       使用默认的双缓冲机制，否则窗口动画可能有花屏现象
 */
#define WITH_THREE_FB 1

/**
 * 如果FLASH空间较小，不足以放大字体文件时，请定义本宏
 *
 */
#define WITH_MINI_FONT 1

/**
 * 如果启用VGCANVAS，而且没有OpenGL硬件加速，请定义本宏
 *
 */
#define WITH_NANOVG_AGGE 1

/**
 * 识别当前使用的屏幕分辨率，定义宏 LCD_W LCD_H
 *
 */
#ifdef AW_DEBUG
#include "aw_prj_params.h"
#if defined(AW_DEV_HW480272F)
#define LCD_W 480
#define LCD_H 272
#elif defined(AW_DEV_HW800480F)
#define LCD_W 800
#define LCD_H 480
#elif defined(AW_DEV_HWCAP480272F)
#define LCD_W 480
#define LCD_H 272
#else
//"not supported"
#endif
#endif
 
#endif/*AWTK_CONFIG_H*/
