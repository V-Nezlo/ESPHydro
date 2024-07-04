#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_PUMPACTUATOR
#define LV_ATTRIBUTE_IMG_PUMPACTUATOR
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_PUMPACTUATOR uint8_t PumpActuator_map[] = {
  0x00, 0x00, 0x00, 0x01, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x5d, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0xcb, 	/*Color of index 2*/
  0x00, 0x00, 0x00, 0xfc, 	/*Color of index 3*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa0, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 
  0x00, 0x00, 0x00, 0x06, 0xaa, 0xaa, 0xaa, 0xab, 0xf0, 
  0x00, 0x00, 0x00, 0x6f, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0x00, 0x00, 0x01, 0xff, 0xef, 0xff, 0xee, 0xef, 0xf0, 
  0x00, 0x00, 0x07, 0xf8, 0x00, 0x2f, 0xd0, 0x01, 0xf0, 
  0x00, 0x00, 0x1f, 0xd0, 0x00, 0x07, 0xf0, 0x01, 0xf0, 
  0x00, 0x00, 0x2f, 0x40, 0x00, 0x01, 0xf8, 0x01, 0xf0, 
  0x00, 0x00, 0x7e, 0x00, 0x28, 0x00, 0xbe, 0xaa, 0xf0, 
  0x00, 0x00, 0xbc, 0x00, 0x7d, 0x00, 0x3f, 0xff, 0xf0, 
  0x00, 0x00, 0xbc, 0x00, 0xff, 0x00, 0x3f, 0xff, 0xf0, 
  0x0a, 0x40, 0xbc, 0x01, 0xff, 0x40, 0x3e, 0x12, 0xf0, 
  0x0f, 0x84, 0xb8, 0x03, 0xff, 0xc0, 0x3e, 0x01, 0xa0, 
  0x0f, 0xff, 0xfc, 0x03, 0xff, 0xc0, 0x3e, 0x00, 0x00, 
  0x0f, 0xff, 0xfc, 0x03, 0xff, 0xc0, 0x3e, 0x00, 0x00, 
  0x0f, 0xaa, 0xbe, 0x00, 0xff, 0x00, 0xfd, 0x00, 0x00, 
  0x0f, 0x40, 0x2f, 0x40, 0x14, 0x01, 0xf8, 0x00, 0x00, 
  0x0f, 0x40, 0x1f, 0xd0, 0x00, 0x07, 0xf4, 0x00, 0x00, 
  0x0f, 0x40, 0x07, 0xfc, 0x00, 0x2f, 0xd0, 0x00, 0x00, 
  0x0f, 0xfb, 0xbb, 0xff, 0xfb, 0xff, 0x40, 0x00, 0x00, 
  0x0f, 0xff, 0xff, 0xff, 0xff, 0xf9, 0x00, 0x00, 0x00, 
  0x0f, 0xea, 0xaa, 0xaa, 0xaa, 0x90, 0x00, 0x00, 0x00, 
  0x0f, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0a, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t PumpActuator = {
  .header.cf = LV_IMG_CF_INDEXED_2BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 36,
  .header.h = 36,
  .data_size = 340,
  .data = PumpActuator_map,
};
