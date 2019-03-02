/**
 * file:   main_loop_aworks_raw.c
 * Author: AWTK Develop Team
 * brief:  main loop for aworks
 *
 * copyright (c) 2018 - 2018 Guangzhou ZHIYUAN Electronics Co.,Ltd.
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

#include "aw_ts.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_emwin_fb.h"
#include "base/idle.h"
#include "base/timer.h"
#include "aw_prj_params.h"
#include "lcd/lcd_mem_bgr565.h"
#include "main_loop/main_loop_simple.h"

/*----------------------------------------------------------------------------*/
/* ������������Ϣ����                                                         */
/*----------------------------------------------------------------------------*/

static struct aw_ts_state s_ts_state = {0};

#define TS_STACK_SIZE 1 * 1024
aw_local void __ts_task_entry(void *p_arg)
{
  int tsret = 0;
  struct aw_ts_state ts_state;

  while (1) {
    memset(&ts_state, 0x00, sizeof(ts_state));
    tsret = aw_ts_exec(p_arg, &ts_state, 1);

    if (tsret >= 0) {
      s_ts_state = ts_state;
    }
    aw_mdelay(20);
  }
}

static void ts_task_init(aw_ts_id sys_ts) {
  /* ��������ʵ�壬����ջ�ռ��СΪ4096  */
  AW_TASK_DECL_STATIC(ts_task, TS_STACK_SIZE);

  AW_TASK_INIT(ts_task,      /* ����ʵ�� */
               "ts_task",   /* �������� */
               4,             /* �������ȼ� */
               TS_STACK_SIZE, /* �����ջ��С */
               __ts_task_entry,  /* ������ں��� */
               sys_ts);         /* ������ڲ��� */

  AW_TASK_STARTUP(ts_task); /* �������� */
}

static aw_ts_id ts_app_init(void) {
#if defined(AW_DEV_HW480272F)
  char TS_SERVER_ID[] = "480x272";
#elif defined(AW_DEV_HW800480F)
  char TS_SERVER_ID[] = "480x272";
#elif defined(AW_DEV_HWCAP480272F)
  char TS_SERVER_ID[] = "480x272";
#else
//"not supported"
#endif

  aw_ts_id sys_ts = aw_ts_serv_id_get(TS_SERVER_ID, 0, 0);
  return_value_if_fail(sys_ts != NULL, NULL);
  
#if defined(AW_DEV_BU21029MUV) || defined(AW_DEV_IMX1050_TS)
  /* ���败������Ҫ��ȡУ׼����, �����������У׼���� */
  return_value_if_fail(aw_ts_calc_data_read(sys_ts) == AW_OK, NULL);
#else
  /* ��������Ҫ����XYת�� */
  aw_ts_set_orientation(sys_ts, AW_TS_SWAP_XY);
#endif

  ts_task_init(sys_ts);
  return sys_ts;
}

ret_t platform_disaptch_input(main_loop_t* loop) {
  static aw_ts_id ts_id = NULL;
  if (ts_id == NULL) {
    ts_id = ts_app_init();
  }

  struct aw_ts_state ts_state = s_ts_state;
  if (ts_state.pressed) {
    main_loop_post_pointer_event(loop, ts_state.pressed, ts_state.x,
        ts_state.y);
  } else {
    main_loop_post_pointer_event(loop, ts_state.pressed, ts_state.x,
        ts_state.y);
  }
  return RET_OK;
}

/*----------------------------------------------------------------------------*/
/* frame bufferˢ�²���                                                       */
/*----------------------------------------------------------------------------*/

extern uint32_t* aworks_get_online_fb(void);
extern uint32_t* aworks_get_offline_fb(void);
extern aw_emwin_fb_info_t* aworks_get_fb(void);
extern int aworks_get_fb_size();
static lcd_flush_t s_lcd_flush_default = NULL;

static ret_t lcd_aworks_fb_flush(lcd_t* lcd) {
#if 0  // �Ƿ�ȴ���ֱͬ��
  // aw_emwin_fb_vram_addr_set �� aw_cache_flush �����Ч����ã����еȴ�ʱ��
  aw_emwin_fb_vram_addr_set(aworks_get_fb(), aworks_get_online_fb()); // max 13ms wait
#endif

  if (s_lcd_flush_default != NULL) {
    s_lcd_flush_default(lcd);
  }

  // ���� aw_cache_flush �ܼ�����Ƹ��������⣬��������ȫȥ��
  aw_cache_flush(aworks_get_online_fb(), aworks_get_fb_size()); // max 2ms wait
  return RET_OK;
}

#ifndef WITH_THREE_FB

/*----------------------------------------------------------------------------*/
/* ˫����ģʽ                                                                 */
/*----------------------------------------------------------------------------*/

static ret_t lcd_aworks_begin_frame(lcd_t* lcd, rect_t* dirty_rect) {
  if (lcd_is_swappable(lcd)) {
    lcd_mem_t* mem = (lcd_mem_t*)lcd;
    (void)mem;

#if 0 // ������һ�����ݵ�offline fb��Ϊ����, begin_frame֮��ֻ�������������
    // ��ǰ��awtk�����ʵ�ֻ���: ÿ֡begin_frameʱ�������������һ֡������κϲ�һ��
    // ����, ��ǰ֡����ʱҲ�����һ֡��������Ҳ����һ��, ����������ִ�������memcpy(������һ�����ݵ�offline fb��Ϊ����)
    // ������Ժ�awtk�޸����������, �ͱ���ִ�������memcpy��
    memcpy(mem->offline_fb, mem->online_fb, aworks_get_fb_size());
#endif

#if 0 // �����ô���, offline fb ���հ�, �������Թ۲�ÿ�λ��Ƶ������
    memset(mem->offline_fb, 0, aworks_get_fb_size());
#endif
  }

  return RET_OK;
}

static ret_t lcd_aworks_swap(lcd_t* lcd) {
  lcd_mem_t* mem = (lcd_mem_t*)lcd;

  uint8_t* next_online_fb = mem->offline_fb;
  mem->offline_fb = mem->online_fb;
  mem->online_fb = next_online_fb;

  aw_cache_flush(next_online_fb, aworks_get_fb_size()); // max 2ms wait
  aw_emwin_fb_vram_addr_set(aworks_get_fb(), (uintptr_t)next_online_fb); // max 13ms wait, �ȴ���ֱͬ��������fb
  return RET_OK;
}

lcd_t* platform_create_lcd(wh_t w, wh_t h) {
  lcd_t* lcd = lcd_mem_bgr565_create_double_fb(w, h, (uint8_t*) aworks_get_online_fb(),
      (uint8_t*) aworks_get_offline_fb());

  if (lcd != NULL) {
    // �Ľ�flush����, ÿ��flush�����cache_flush (��ת��Ļ��������flush����)
    s_lcd_flush_default = lcd->flush;
    lcd->flush = lcd_aworks_fb_flush;

    // ʹ��swap����(������Ļ�������swap����)
    lcd->begin_frame = lcd_aworks_begin_frame;
    lcd->swap = lcd_aworks_swap;
  }

  return lcd;
}

#else // WITH_THREE_FB

/*----------------------------------------------------------------------------*/
/* ������ģʽ                                                                 */
/*----------------------------------------------------------------------------*/

AW_MUTEX_DECL(__lock_fblist);
static uint32_t* s_fblist_readys = NULL;
static uint32_t* s_fblist_frees = NULL;
static uint8_t* s_dirty_offline = NULL;  // ָ��mem->offline_fb, ����ǰoffline�����Ƿ��Ѿ���д��

static uint32_t* aworks_fblist_pop_ready() {
  AW_MUTEX_LOCK( __lock_fblist, AW_SEM_WAIT_FOREVER );
  uint32_t* fb = s_fblist_readys;
  s_fblist_readys = NULL;
  AW_MUTEX_UNLOCK(__lock_fblist);
  return fb;
}

static void aworks_fblist_push_ready(uint32_t* fb) {
  AW_MUTEX_LOCK( __lock_fblist, AW_SEM_WAIT_FOREVER );
  assert(s_fblist_readys == NULL);
  s_fblist_readys = fb;
  AW_MUTEX_UNLOCK(__lock_fblist);
}

static uint32_t* aworks_fblist_pop_free() {
  AW_MUTEX_LOCK( __lock_fblist, AW_SEM_WAIT_FOREVER );
  uint32_t* fb = s_fblist_frees;
  s_fblist_frees = NULL;
  AW_MUTEX_UNLOCK(__lock_fblist);
  return fb;
}

static void aworks_fblist_push_free(uint32_t* fb) {
  AW_MUTEX_LOCK( __lock_fblist, AW_SEM_WAIT_FOREVER );
  assert(s_fblist_frees == NULL);
  s_fblist_frees = fb;
  AW_MUTEX_UNLOCK(__lock_fblist);
}

#define SWAP_STACK_SIZE 1 * 1024
aw_local void __swap_task_entry(void *p_arg)
{
  uint32_t* current_vram = (uint32_t*)p_arg;

  while (1) {
    uint32_t* ready = aworks_fblist_pop_ready();
    if (ready) {
      uint32_t* last_online = current_vram;
      aw_emwin_fb_vram_addr_set(aworks_get_fb(), (uintptr_t)(current_vram = ready));
      aworks_fblist_push_free(last_online);
    } else {
      aw_mdelay(2);
    }
  }
}

static ret_t __swap_idle_entry(const idle_info_t* idle) {
  lcd_mem_t* mem = (lcd_mem_t*)idle->ctx;

  // ����Ƿ������offline����, �����, ��ǿ�Ƹ��µ�ready, ��һ��ѭ�������µ�֡ˢ�µ�online
  // ���ҽ�mem->offline_fbָ�����µ�free����
  if (s_dirty_offline) {
    uint32_t* freefb = aworks_fblist_pop_free();
    if (freefb) {
      aw_cache_flush(mem->offline_fb, aworks_get_fb_size());
      aworks_fblist_push_ready((uint32_t*)mem->offline_fb);

      mem->offline_fb = (uint8_t*)freefb;
      s_dirty_offline = NULL;
    }
  }
  return RET_REPEAT;
}

static void aworks_fblist_init(lcd_t* lcd) {
  lcd_mem_t* mem = (lcd_mem_t*)lcd;
  uint32_t* frame_buffer = (uint32_t*)mem->online_fb;
  uint32_t* next_frame_buffer = (uint32_t*)mem->next_fb;
  int fb_size = aworks_get_fb_size();

  assert(frame_buffer && next_frame_buffer);
  memset(next_frame_buffer, 0x00, fb_size);

  AW_MUTEX_INIT(__lock_fblist, AW_SEM_INVERSION_SAFE);
  s_fblist_frees = next_frame_buffer;
  s_fblist_readys = NULL;
  s_dirty_offline = NULL;

  AW_TASK_DECL_STATIC(swap_task, SWAP_STACK_SIZE);
  AW_TASK_INIT(swap_task,      /* ����ʵ�� */
               "swap_task",   /* �������� */
               1,             /* �������ȼ� */
               SWAP_STACK_SIZE, /* �����ջ��С */
               __swap_task_entry,  /* ������ں��� */
               frame_buffer);         /* ������ڲ��� */
  AW_TASK_STARTUP(swap_task); /* �������� */

  // ����idle����(ͬgui�߳�), ����Ƿ�����������offline����, ��ˢ�µ�online
  idle_add(__swap_idle_entry, lcd);
}

static ret_t lcd_aworks_begin_frame(lcd_t* lcd, rect_t* dirty_rect) {
  if (lcd_is_swappable(lcd)) {
    lcd_mem_t* mem = (lcd_mem_t*)lcd;
    (void)mem;
    s_dirty_offline = NULL; // �µ�һ֡��ʼ����
  }
  return RET_OK;
}

static ret_t lcd_aworks_swap(lcd_t* lcd) {
  lcd_mem_t* mem = (lcd_mem_t*)lcd;

  uint32_t* freefb = aworks_fblist_pop_free();
  if (freefb) {
    aw_cache_flush(mem->offline_fb, aworks_get_fb_size());
    aworks_fblist_push_ready((uint32_t*)mem->offline_fb);

    mem->offline_fb = (uint8_t*)freefb;
  } else {
    // ��ǰ֡�Ѿ������µ�offline, ������û�п��л���free������(����swap������)
    // ���Ե�ǰofflineû�б���ʱ���µ�ready��, �ᵼ�����һ֡����������offline��
    // ��Ҫswap�̼߳�������ǿ�ƽ����µ�offline�Ż�ready, ��֤���µ����ݸ��µ�online
    // 
    s_dirty_offline = mem->offline_fb;
  }
  return RET_OK;
}

lcd_t* platform_create_lcd(wh_t w, wh_t h) {
  lcd_t* lcd = lcd_mem_bgr565_create_three_fb(w, h, (uint8_t*) aworks_get_online_fb(),
        (uint8_t*) aworks_get_offline_fb(), aw_mem_align(aworks_get_fb_size(), AW_CACHE_LINE_SIZE));

  if (lcd != NULL) {
    // �Ľ�flush����, ÿ��flush�����cache_flush (��ת��Ļ��������flush����)
    s_lcd_flush_default = lcd->flush;
    lcd->flush = lcd_aworks_fb_flush;

    // ʹ��swap����(������Ļ�������swap����)
    lcd->begin_frame = lcd_aworks_begin_frame;
    lcd->swap = lcd_aworks_swap;
  }

  aworks_fblist_init(lcd);
  return lcd;
}

#endif // WITH_THREE_FB

#include "main_loop/main_loop_raw.inc"
