// F256 System control registers
//
// These controls LEDs, buzzer, LFSR (pseudo random number) and holds
// identity values about the machine.

#ifndef __F256_SYSTEM_H__
#define __F256_SYSTEM_H__

#include <stdint.h>

struct _SystemControl {
  union {
    struct {
      uint8_t                    : 6; // read
      uint8_t SD_inserted        : 1;
      uint8_t SD_write_protected : 1;
    };
    struct {
      uint8_t power_led    : 1;       // write
      uint8_t sd_led       : 1;
      uint8_t status0_led  : 1;
      uint8_t status1_led  : 1;
      uint8_t buzzer       : 1;
      uint8_t              : 2;
      uint8_t manual_reset : 1;
    };
    uint8_t control0;
  };
  union {
    struct {
      uint8_t status_led0  : 1;
      uint8_t status_led1  : 1;
      uint8_t              : 2;
      uint8_t flash_rate0  : 2;
      uint8_t flash_rate1  : 2;
    };
    uint8_t control1;
  };
  uint8_t control2;
  uint8_t control3;
  union {
    uint16_t seed;
    uint16_t random;
  };
  union {
    struct {                        // read
      uint8_t              : 7;
      uint8_t lfsr_done    : 1;
    };
    struct {                        // write
      uint8_t lfsr_enable  : 1;
      uint8_t seed_write   : 1;
      uint8_t              : 6;
    };
  };
  uint8_t machine_id;
  char    hardware_version[2];
  uint8_t chip_sub_version[2];
  uint8_t chip_version[2];
  uint8_t chip_number[2];
};

// Status led flash rate
typedef enum StatusFlashRate {
  FlashRate_1sec, FlashRate_0_5sec, FlashRate_0_4sec, FlashRate_0_2sec
} StatusFlashRate_t;

typedef enum MachineID {
  Machine_FMX, Machine_C256U, Machine_C256JR, Machine_A2560DEV,
  Machine_GenX, Machine_C256UPlus, Machine_Reserved1, Machine_Reserved2,
  Machine_A2560X, Machine_A2560U, Machine_A2560M, Machine_A2560K
} MachineID_t;

// The DIP switch which is intended for setting boot configuration.
struct _DIP_switch {
  uint8_t boot0            : 1;
  uint8_t boot1            : 1;
  uint8_t boot2            : 1;
  uint8_t boot3            : 1;
  uint8_t user0            : 1;
  uint8_t user1            : 1;
  uint8_t user2            : 1;
  uint8_t gamma_correction : 1;
};

#endif // __F256_SYSTEM_H__
