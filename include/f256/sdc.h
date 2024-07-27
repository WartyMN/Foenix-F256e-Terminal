#ifndef __SDC_H__
#define __SDC_H__

#include <stdint.h>

struct _SDC {
  uint8_t version;
  union {
    struct {
      uint8_t reset : 1;
      uint8_t       : 7;
    };
    uint8_t master_control;
  };
  union {
    struct {
      uint8_t transfer_type : 2;
      uint8_t               : 6;
    };
  };
  union {
    struct {
      uint8_t start_transation : 1;
      uint8_t                  : 7;
    };
    uint8_t transfer_control;
  };
  union {
    struct {
      uint8_t busy : 1;
      uint8_t      : 7;
    };
    uint8_t transfer_status;
  };
  union {
    struct {
      uint8_t sd_init_error  : 2;
      uint8_t sd_read_error  : 2;
      uint8_t sd_write_error : 2;
      uint8_t                : 2;
    };
    uint8_t transfer_error;
  };
  uint8_t  direct_data;
  uint32_t address;
  uint8_t  clock_del;
  uint8_t  fifo_read;
  uint8_t  fifo_read_count_hi;
  uint8_t  fifo_read_count_lo;
  union {
    struct {
      uint8_t empty_fifo_read : 1;
      uint8_t                 : 7;
    };
    uint8_t fifo_read_control;
  };
  uint8_t  fifo_write;
  union {
    struct {
      uint8_t empty_fifo_write : 1;
      uint8_t                  : 7;
    };
    uint8_t fifo_write_control;
  };
};

#endif // __SDC_H__
