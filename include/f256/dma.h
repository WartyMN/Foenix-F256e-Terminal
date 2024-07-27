// F256 DMA
//

#ifndef __F256_DMA_H__
#define __F256_DMA_H__

#include <stdint.h>

struct _DMA {
  union {
    uint8_t     control;
    struct {
      uint8_t   enable           : 1;
      uint8_t   mode_1d_2d       : 1;
      uint8_t   fill             : 1;
      uint8_t   interrupt_enable : 1;
      uint8_t                  : 3;
      uint8_t   start_transfer   : 1;
    };
  };
  union {
    uint8_t     data_to_write;  // write only
    union {
      uint8_t   status;         // read only
      struct {
        uint8_t                : 7;
        uint8_t in_progress    : 1;
      };
    };
  };
  uint8_t       reserved_0;
  uint8_t       reserved_1;

  uint32_t      source_addy;
  uint32_t      dest_addy;
  union {
    uint32_t    size_1d;        // 1D mode
    struct {
      uint16_t  size_x;         // 2D mode
      uint16_t  size_y;
    };
  };
  uint16_t      stride_x;       // stride in 2D mode
  uint16_t      stride_y;

  uint8_t       reserved_5;
  uint8_t       reserved_6;
  uint8_t       reserved_7;
  uint8_t       reserved_8;
};

#endif // __F256_DMA_H__
