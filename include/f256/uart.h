// F256 UART
//

#ifndef __F256_UART_H__
#define __F256_UART_H__

#include <stdint.h>

struct _UART {
  union {
    uint16_t baud;     // when DLAB=1
    struct {
      union {          // data register, send and receive
        uint8_t data;
        uint8_t rhr;
        uint8_t thr;
      };
    };
  };
  union {          // interrupt enable
    struct {
      uint8_t interrupt_enable_receive      : 1;
      uint8_t interrupt_enable_transmit     : 1;
      uint8_t interrupt_enable_line_status  : 1;
      uint8_t interrupt_enable_modem_status : 1;
    };
    uint8_t ier;
#define INTERRUPT_ENABLE_RECEIVE      0x01
#define INTERRUPT_ENABLE_TRANSMIT     0x02
#define INTERRUPT_ENABLE_LINE_STATUS  0x04
#define INTERRUPT_ENABLE_MODEM_STATUS 0x08
  };
  union {
    union {
      struct {
        uint8_t interrupt_status   : 1;
        uint8_t interrupt_priority : 3;
        uint8_t                    : 2;
        uint8_t fifos_enabled      : 2;     // ????
      };
      uint8_t isr;       // interrupt status (read)
    };
    union {
      struct {
        uint8_t fifo_enable         : 1;
        uint8_t reset_fifo_receive  : 1;
        uint8_t reset_fifo_transmit : 1;
        uint8_t dma_mode            : 1;
        uint8_t                     : 2;
        uint8_t receive_trigger     : 2;
      };
      uint8_t fcr;       // FIFO control (write)
    };
  };

  uint8_t lcr;         // line control
#define WORD_LENGTH_5        0x00
#define WORD_LENGTH_6        0x01
#define WORD_LENGTH_7        0x02
#define WORD_LENGTH_8        0x03
#define STOP_BITS_1          0x00
#define STOP_BITS_2          0x04
#define NO_PARITY            0x00
#define ODD_PARITY           0x08
#define EVEN_PARITY          0x18
#define FORCE_PARITY_1       0x28
#define FORCE_PARITY_0       0x38
#define SET_BREAK            0x40
#define DIVISOR_LATCH_ENABLE 0x80

  uint8_t mcr;         // modem control
#define MCR_DTR              0x01
#define MCR_RTS              0x02
#define MCR_OUT1             0x04
#define MCR_OUT2             0x08
#define MCR_LOOP_BACK        0x10
#define MCR_AFE              0x20

  uint8_t lsr;         // line status
#define receive_data_ready   0x01
#define overrun_error        0x02
#define parity_error         0x04
#define framing_error        0x08
#define break_interrupt      0x10
#define transmit_hold_empty  0x20
#define transmit_empty       0x40
#define fifo_data_error      0x80

  uint8_t msr;         // modem status
  uint8_t spr;         // scratch pad
};

#define BAUD_300       0x1472
#define BAUD_600       0x0a39
#define BAUD_1200      0x051d
#define BAUD_2400      0x028e
#define BAUD_4800      0x0147
#define BAUD_9600      0x00a4
#define BAUD_19200     0x0052
#define BAUD_31250     0x0032
#define BAUD_38400     0x0029
#define BAUD_57600     0x001b
#define BAUD_115200    0x000d

#endif // __F256_UART_H__
