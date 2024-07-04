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

#ifndef LV_ATTRIBUTE_IMG_DRIP
#define LV_ATTRIBUTE_IMG_DRIP
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_DRIP uint8_t Drip_map[] = {
  0x00, 0x00, 0x00, 0x02, 	/*Color of index 0*/
  0x04, 0x03, 0x03, 0x7c, 	/*Color of index 1*/
  0x04, 0x03, 0x03, 0xbe, 	/*Color of index 2*/
  0x04, 0x03, 0x03, 0xfa, 	/*Color of index 3*/

  0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x50, 0x00, 
  0xd5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xb4, 0x00, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1d, 0x00, 
  0xea, 0xaa, 0xaa, 0xa8, 0x6a, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x5a, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x92, 0xaa, 0xaa, 0xaa, 0xaa, 0x94, 0x07, 0x00, 
  0x55, 0x55, 0x55, 0x5d, 0x75, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x57, 0x5d, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xd7, 0x55, 0x55, 0x55, 0x55, 0xae, 0x03, 0x40, 
  0x00, 0x00, 0x00, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x00, 0x03, 0x81, 0x80, 
  0x00, 0x00, 0x00, 0x0d, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x80, 
  0x00, 0x00, 0x00, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x0d, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x2a, 0xaa, 0xaa, 0xaf, 0xfa, 0xaa, 0xaa, 0xa8, 0x00, 0x00, 0x00, 0x0a, 0xaa, 0xaa, 0xab, 0xfe, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x02, 0xaa, 0xaa, 0xaa, 0xff, 0xaa, 0xaa, 0xaa, 0x80, 0x00, 0x91, 0x80, 
  0xf5, 0x55, 0x55, 0x5a, 0xa5, 0x55, 0x55, 0x5e, 0x00, 0x00, 0x00, 0x3d, 0x55, 0x55, 0x56, 0xa9, 0x55, 0x55, 0x57, 0xc0, 0x00, 0x00, 0x0b, 0x55, 0x55, 0x55, 0xaa, 0x55, 0x55, 0x55, 0xf0, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x07, 0xd0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x01, 0xf4, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x0d, 0xd0, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x02, 0xa8, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0xb7, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x0c, 0x70, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x03, 0x5c, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x1c, 0x30, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x07, 0x0d, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01, 0xc3, 0x40, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x28, 0x34, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x07, 0x09, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01, 0xc2, 0x80, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x28, 0x34, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x06, 0x09, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x01, 0xc2, 0x80, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x1d, 0x70, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x07, 0x5d, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0xe7, 0x40, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x07, 0x90, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x01, 0xe4, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xe9, 0x00, 0x00, 0x1b, 0xe4, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x3e, 0x40, 0x00, 0x02, 0xf9, 0x00, 0x00, 0x0a, 0xc0, 0x00, 0x00, 0x0b, 0x90, 0x00, 0x01, 0x7e, 0x40, 0x00, 0x05, 0xf0, 0x00, 0x91, 0x80, 
  0xeb, 0x90, 0x01, 0xb9, 0x5f, 0x40, 0x06, 0xd7, 0x00, 0x00, 0x00, 0x36, 0xb4, 0x00, 0x6e, 0x57, 0xd0, 0x00, 0xf9, 0xc0, 0x00, 0x00, 0x0a, 0x7e, 0x00, 0x0b, 0x95, 0xf8, 0x00, 0x2e, 0x74, 0x00, 0x91, 0x80, 
  0xc0, 0xbe, 0x6f, 0x80, 0x01, 0xf9, 0xbe, 0x07, 0x00, 0x00, 0x00, 0x30, 0x1f, 0x9a, 0xe0, 0x00, 0x7e, 0x9f, 0x81, 0xc0, 0x00, 0x00, 0x09, 0x07, 0xe9, 0xf9, 0x00, 0x1f, 0xa7, 0xe4, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x05, 0x94, 0x00, 0x00, 0x16, 0x90, 0x03, 0x00, 0x00, 0x00, 0x34, 0x01, 0x69, 0x00, 0x00, 0x05, 0xa4, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x5a, 0x40, 0x00, 0x00, 0x69, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x91, 0x80, 
  0xba, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xad, 0x00, 0x00, 0x00, 0x2e, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xab, 0x80, 0x00, 0x00, 0x07, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x00, 0x91, 0x80, 
  0x15, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x00, 0x00, 0x00, 0x05, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x00, 0x00, 0x00, 0x01, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x40, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x80, 
  0x15, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x40, 0x00, 0x91, 0x80, 
  0xba, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xd0, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xfe, 0x00, 0x00, 0x7f, 0xf9, 0x00, 0x01, 0xff, 0xe4, 0x00, 0x06, 0xff, 0x90, 0x00, 0x1b, 0xff, 0x40, 0x00, 0x2f, 0xfd, 0x00, 0x00, 0xbf, 0xf8, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x07, 0xf4, 0x00, 0x91, 0x80, 
  0xc6, 0xf4, 0x06, 0xd0, 0x17, 0x90, 0x1b, 0x40, 0x2f, 0x40, 0x6e, 0x40, 0xbd, 0x41, 0xb9, 0x01, 0xf8, 0x05, 0xf4, 0x07, 0xe4, 0x0b, 0xd0, 0x1b, 0x90, 0x2f, 0x40, 0x6e, 0x40, 0xbd, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x1f, 0xfe, 0x00, 0x00, 0xbf, 0xf8, 0x00, 0x01, 0xff, 0xd4, 0x00, 0x07, 0xff, 0x50, 0x00, 0x1b, 0xfe, 0x40, 0x00, 0x6f, 0xf9, 0x00, 0x01, 0xbf, 0xe4, 0x00, 0x02, 0xff, 0x90, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x04, 0x40, 0x00, 0x00, 0x11, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0xa1, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x55, 0x55, 0x55, 0x55, 0x74, 0x00, 0x91, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0xaa, 0xaa, 0xaa, 0xaa, 0xb0, 0x00, 0xd1, 0x80, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xaa, 0x09, 0x00, 0x00, 0x00, 0x00, 0x78, 0x47, 0x83, 0x40, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x57, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfd, 0x07, 0x00, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0xab, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x1d, 0x00, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x57, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x75, 0x55, 0xb8, 0x00, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa5, 0x57, 0xae, 0x00, 0x00, 0x00, 0x00, 0x7e, 0xaa, 0x80, 0x00, 
  0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xaa, 0x09, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 
  0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x11, 0x11, 0x11, 0x11, 0xf0, 0x00, 0x00, 0x00, 
  0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
};

const lv_img_dsc_t Drip = {
  .header.cf = LV_IMG_CF_INDEXED_2BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 133,
  .header.h = 78,
  .data_size = 2668,
  .data = Drip_map,
};
