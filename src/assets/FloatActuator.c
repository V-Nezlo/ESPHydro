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

#ifndef LV_ATTRIBUTE_IMG_FLOATACTUATOR
#define LV_ATTRIBUTE_IMG_FLOATACTUATOR
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_FLOATACTUATOR uint8_t FloatActuator_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0x40, 	/*Color of index 1*/
  0x00, 0x00, 0x00, 0x92, 	/*Color of index 2*/
  0x00, 0x00, 0x00, 0xfd, 	/*Color of index 3*/

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x01, 0x55, 0x40, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0b, 0xff, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xaa, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0x00, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0xaa, 0xfc, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x2f, 0xff, 0xf8, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x0b, 0xff, 0xf0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xfb, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xeb, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xf7, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xeb, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xf7, 0xd0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0xfb, 0xe0, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x7f, 0xff, 0xfd, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xfa, 0xaa, 0xaf, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xf4, 0x00, 0x1f, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0xfa, 0xaa, 0xaf, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xbf, 0xff, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x05, 0x55, 0x54, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t FloatActuator = {
  .header.cf = LV_IMG_CF_INDEXED_2BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 36,
  .header.h = 36,
  .data_size = 340,
  .data = FloatActuator_map,
};
