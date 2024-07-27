// F256 Versatile Interface Adapter (6522 VIA)
//

#ifndef __F256_VIA_H__
#define __F256_VIA_H__

#include <stdint.h>

struct _VIA {
  uint8_t a;      // register A
  uint8_t b;      // register B
  uint8_t ddrb;   // data direction register B
  uint8_t ddra;   // data direction register A
  uint8_t t1cl;
  uint8_t t1ch;
  uint8_t t1ll;
  uint8_t t1lh;
  uint8_t t2cl;
  uint8_t t2ch;
  uint8_t sr;
  union {
    struct {
      uint8_t latch_enable_a         : 1;
      uint8_t latch_enable_b         : 1;
      uint8_t shift_register_control : 3;
      uint8_t timer2_control         : 1;
      uint8_t timer1_control         : 2;
    };
    uint8_t acr;
  };
  union {
    struct {
      uint8_t ca1 : 1;   // interrupt control
      uint8_t ca2 : 3;   // control
      uint8_t cb1 : 1;   // interrupt control
      uint8_t cb2 : 3;   // control
    };
    uint8_t pcr;
  };
  union {
    struct {
      uint8_t interrupt_flag_ca2       : 1;
      uint8_t interrupt_flag_ca1       : 1;
      uint8_t interrupt_flag_shift_reg : 1;
      uint8_t interrupt_flag_cb2       : 1;
      uint8_t interrupt_flag_cb1       : 1;
      uint8_t interrupt_flag_timer2    : 1;
      uint8_t interrupt_flag_timer1    : 1;
      uint8_t interrupt_flag_irq       : 1;
    };
    uint8_t ifr;
  };

  union {
    struct {
      uint8_t interrupt_enable_ca2       : 1;
      uint8_t interrupt_enable_ca1       : 1;
      uint8_t interrupt_enable_shift_reg : 1;
      uint8_t interrupt_enable_cb2       : 1;
      uint8_t interrupt_enable_cb1       : 1;
      uint8_t interrupt_enable_timer2    : 1;
      uint8_t interrupt_enable_timer1    : 1;
      uint8_t interrupt_enable_set_clear : 1;
    };
    uint8_t ier;
  };
  uint8_t a_;     // same as 'a', but no handshake
};

enum via_shift_register_control {
  ShiftRegisterDisabled,
  ShiftInUnderControlOfT2, ShiftInUnderControlOfPhi2,
  ShiftInUnderControlOfExternalClock,
  ShiftOutFreeRunningAtT2Rate,
  ShiftOutUnderControlOfT2, ShiftOutUnderControlOfPhi2,
  ShiftOutUnderControlOfExternalClock,
};

enum via_timer1_control {
  TimedInterruptOnLoadT1_NoPB7, ContinuousInterruptsT1_NoPB7,
  TimedInterruptOnLoadT1_PB7, ContinuousInterruptsT1_PB7
};

enum via_interrupt_control {
  NegativeActiveEdge, PositiveActiveEdge
};

enum via_control {
  InputNegativeActiveEdge, IndependentInterruptInputNegativeEdge,
  InputPositiveActiveEdge, IndependentInterruptInputPositiveEdge,
  HandshakeOutput, PulseOutput, LowOutput, HighOutput
};

#endif // __F256_VIA_H__
