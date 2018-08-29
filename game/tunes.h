#ifndef FXM_TUNES_H
#define FXM_TUNES_H

#ifndef EMULATED

#include <Arduino.h>
#include "music.h"

#define TUNE(A, B) extern const uint8_t B[] MUSICMEM; \
                   static inline uint32_t B##_addr()  \
                   {                                  \
                       return pgm_get_far_address(B); \
                   }
#include "tunes_list.h"
#undef TUNE

#endif

#endif
