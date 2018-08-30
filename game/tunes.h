#ifndef FXM_TUNES_H
#define FXM_TUNES_H

#ifndef EMULATED

#include <Arduino.h>
#include "music.h"

typedef uint32_t (*TuneAddr)();

#define TUNE_ADDR(A) A##_addr

#define TUNE(A, B) extern const uint8_t B[] MUSICMEM; \
                   uint32_t TUNE_ADDR(B)();
#include "tunes_list.h"
#undef TUNE

#endif

#endif
