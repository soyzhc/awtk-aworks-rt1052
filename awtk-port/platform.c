/**
 * File:   platform.c
 * Author: AWTK Develop Team
 * Brief:  platform dependent function of aworks
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2018-05-23 li xianjing <xianjimli@hotmail.com> created
 *
 */

#include "aw_system.h"
#include "aw_delay.h"
#include "base/timer.h"
#include "tkc/mem.h"
#include "tkc/date_time.h"

#include <time.h>
#include <sys/time.h>
static ret_t date_time_get_now_impl(date_time_t* dt) {
  time_t now = time(0);
  struct tm* t = localtime(&now);

  if (t) {
    dt->second = t->tm_sec;
    dt->minute = t->tm_min;
    dt->hour = t->tm_hour;
    dt->day = t->tm_mday;
    dt->month = t->tm_mon + 1;
    dt->year = t->tm_year + 1900;
  } else {
    dt->second = 0;
    dt->minute = 0;
    dt->hour = 0;
    dt->day = 11;
    dt->month = 11;
    dt->year = 2018;
  }

  return RET_OK;
}

uint32_t get_time_ms() {
  return aw_sys_tick_get();
}

void sleep_ms(uint32_t ms) {
  aw_mdelay(ms);
}

#define TK_MEM_SIZE 6 * 1024 * 1024

ret_t platform_prepare(void) {
#ifndef HAS_STD_MALLOC
  uint32_t* mem = (uint32_t*) aw_mem_alloc(TK_MEM_SIZE);
  return_value_if_fail(mem != NULL, RET_OOM);

  tk_mem_init(mem, TK_MEM_SIZE);
#endif

  date_time_set_impl(date_time_get_now_impl);
  return RET_OK;
}
