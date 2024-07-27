; Pending Interrupt (Read and Write Back to Clear)
INT_PENDING_REG0      	.equlab 	0xF01660 ;
INT_PENDING_REG1      	.equlab 	0xF01661 ;
INT_PENDING_REG2      	.equlab 	0xF01662 ; IEC Signals Interrupt
INT_PENDING_REG3      	.equlab 	0xF01663 ; NOT USED
; Polarity Set
INT_POL_REG0      		.equlab 	0xF01664 ;
INT_POL_REG1      		.equlab 	0xF01665 ;
INT_POL_REG2      		.equlab 	0xF01666 ; IEC Signals Interrupt
INT_POL_REG3      		.equlab 	0xF01667 ; NOT USED
; Edge Detection Enable
INT_EDGE_REG0      		.equlab 	0xF01668 ;
INT_EDGE_REG1      		.equlab 	0xF01669 ;
INT_EDGE_REG2      		.equlab 	0xF0166A ; IEC Signals Interrupt
INT_EDGE_REG3      		.equlab 	0xF0166B ; NOT USED
; Mask
INT_MASK_REG0      		.equlab 	0xF0166C ;
INT_MASK_REG1      		.equlab 	0xF0166D ;
INT_MASK_REG2      		.equlab 	0xF0166E ; IEC Signals Interrupt
INT_MASK_REG3      		.equlab 	0xF0166F ; NOT USED

; Interrupt Bit Definition
; Register Block 0
JR0_INT00_SOF        .equlab 	0x01  ;Start of Frame @ 60FPS or 70hz (depending on the Video Mode)
JR0_INT01_SOL        .equlab 	0x02  ;Start of Line (Programmable)
JR0_INT02_KBD        .equlab 	0x04  ;PS2 Keyboard
JR0_INT03_MOUSE      .equlab 	0x08  ;PS2 Mouse 
JR0_INT04_TMR0       .equlab 	0x10  ;Timer0
JR0_INT05_TMR1       .equlab 	0x20  ;Timer1
JR0_INT06_RSVD0      .equlab 	0x40  ;Reserved 
JR0_INT07_CRT        .equlab 	0x80  ;Cartridge
; Register Block 1
JR1_INT00_UART       .equlab 	0x01  ;UART
JR1_INT01_TVKY2      .equlab 	0x02  ;TYVKY NOT USED
JR1_INT02_TVKY3      .equlab 	0x04  ;TYVKY NOT USED
JR1_INT03_TVKY4      .equlab 	0x08  ;TYVKY NOT USED
JR1_INT04_RTC        .equlab 	0x10  ;Real Time Clock
JR1_INT05_VIA0       .equlab 	0x20  ;VIA0 (Jr & K)
JR1_INT06_VIA1       .equlab 	0x40  ;VIA1 (K Only) - Local Keyboard
JR1_INT07_SDCARD     .equlab 	0x80  ;SDCard Insert Int
; Register Block 1
JR2_INT00_IEC_DAT    .equlab 	0x01  ;IEC_DATA_i
JR2_INT01_IEC_CLK    .equlab 	0x02  ;IEC_CLK_i
JR2_INT02_IEC_ATN    .equlab 	0x04  ;IEC_ATN_i
JR2_INT03_IEC_SREQ   .equlab 	0x08  ;IEC_SREQ_i
JR2_INT04_RSVD1      .equlab 	0x10  ;Reserved
JR2_INT05_RSVD2      .equlab 	0x20  ;Reserved
JR2_INT06_RSVD3      .equlab 	0x40  ;Reserved
JR2_INT07_RSVD4      .equlab 	0x80  ;Reserved