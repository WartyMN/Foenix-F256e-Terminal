                .rtmodel codemodel,"large"
                .rtmodel dataModel,"medium"
                .rtmodel version, "1"
                .rtmodel cpu, "*"	

;                .public fontset_std
;                .public fontset_ja
;                .public fontset_ansi
;                .public fontset_ibm_ansi

                .public __low_level_init

#include "f256xe_macros.h"	
#include "define/interrupt_def.asm"
#include "define/Vicky_The_Fourth_Def.asm"

VECTORS_BEGIN   .equlab 	0x00FFFA ;0 Byte  Interrupt vectors
VECTOR_NMI      .equlab 	0x00FFFA ;2 Bytes Emulation mode interrupt handler
VECTOR_RESET    .equlab 	0x00FFFC ;2 Bytes Emulation mode interrupt handler
VECTOR_IRQ      .equlab 	0x00FFFE ;2 Bytes Emulation mode interrupt handler	

              .section code, noreorder
; note to self 
; With calypsi, # = 8bits, ## means 16bits
; dp: 8bits
; abs: 16bits
; long: 24bits
__low_level_init:
                ; the Call from cstartup gets us in X:A @ 16bits
                ; INIT 
				jsl IRQ_Init_Mask		; Just make sure all masks are off and the Pending registers are cleared
				jsl	InitFONT_Mem		; in the RevA of the Board the FONT Memory is empty
				jsl Init_Gamma			; Init Gamma table with 1.8 Value
				jsl Init_Text_LUT		; Init the Text Color Table				
				jsl TinyVky_Init		; Keeping the addressing long (just in case)
                ; Set the Backgroud Color
                jsl Fill_Color			; Set the Text Background Color 
                ; Fill the Screen with Spaces
                jsl Clear_Screen    	;
				jsl SplashText
                setaxl                  ; Make sure to go back to Cstartup with X16/A16
                rtl


;*********************************************************************
;************************** Interrupt_Mask ***************************
;*********************************************************************
IRQ_Init_Mask:

				sei							; Disable IRQ handling
				setas 
				lda #0xff					; Clear Everything in the Interrupt Controller Section - So no Spurious Interrupt happens
				;sta long: INT_EDGE_REG0	; MB: f256 ref manual recommends leaving as is
				;sta long: INT_EDGE_REG1	; MB: f256 ref manual recommends leaving as is
				sta long: INT_EDGE_REG2		; MB: f256 ref manual implies this is more important for IEC stuff
				sta long: INT_EDGE_REG3		; MB: reg3 is unused, but...
				
				;sta long: INT_MASK_REG0	; MB: want to allow PS/2 keyboard events, so will set below...				
				sta long: INT_MASK_REG1		; MB: do not need to allow F256K machines to get VIA interrupts, if just doing keyboard. can do VICKY SOF.
				sta long: INT_MASK_REG2
				sta long: INT_MASK_REG3	
				lda long: INT_PENDING_REG0
				sta long: INT_PENDING_REG0
				lda long: INT_PENDING_REG1
				sta long: INT_PENDING_REG1
				lda long: INT_PENDING_REG2
				sta long: INT_PENDING_REG2
				lda long: INT_PENDING_REG3
				sta long: INT_PENDING_REG3
				
				; MB: allow PS/2 keyboard interrupts
				and #~JR0_INT02_KBD
				; MB: allow VICKY SOF interrupts
;				and #~JR0_INT00_SOF
;				and #~(JR0_INT02_KBD | JR0_INT00_SOF)

;				sec
;				sbc #JR0_INT02_KBD
;				sbc #JR0_INT00_SOF
				
				sta long: INT_MASK_REG0
				
				; enable interrupts for UART
				lda #0xff
				and #~JR1_INT00_UART
				sta long: INT_MASK_REG1
				
				; MB: Flush the PS/2 keyboard port
				lda #0x10
				sta long: 0xF01640			; VICKY_PS2_CTRL
				lda #0x00
				sta long: 0xF01640			; VICKY_PS2_CTRL
				cli							; Re-enable IRQ handling
				
				rtl
	
;*********************************************************************
;************************** TinyVky_Init  ****************************
;*********************************************************************
TinyVky_Init:
				; .as 
				setas
				lda #Mstr_Ctrl_Text_Mode_En | Mstr_Ctrl_GAMMA_En	; We are still in 8bits mode (during the init) 
				sta long: MASTER_CTRL_REG_L
				; Border Setup 
				lda #Border_Ctrl_Enable
				sta long: BORDER_CTRL_REG
				lda #0x20 ;AAFFEE
				sta long: BORDER_COLOR_B
				lda #0x20 ;AAFFEE
				sta long: BORDER_COLOR_G           
				lda #0x20
				sta long: BORDER_COLOR_R
				lda #8
				sta long: BORDER_X_SIZE
				sta long: BORDER_Y_SIZE
				; Cursor Setup
				lda #Vky_Cursor_Enable | Vky_Cursor_Flash_Rate1 
				sta long: VKY_TXT_CURSOR_CTRL_REG
				lda #219
				sta long: VKY_TXT_CURSOR_CHAR_REG
				lda #28
				sta long: VKY_TXT_CURSOR_COLR_REG
				lda #0
				sta long: VKY_TXT_CURSOR_X_REG_L
				sta long: VKY_TXT_CURSOR_X_REG_H
				sta long: VKY_TXT_CURSOR_Y_REG_H
				lda #5
				sta long: VKY_TXT_CURSOR_Y_REG_L
				rtl

;*********************************************************************
;************************** Init_Text_LUT ****************************
;*********************************************************************
Init_Text_LUT:
                setas
                setxl 
				ldx	##0000
1$: 			lda long:fg_color_lut,x		; get Local Data
                sta long:TEXT_LUT_FG,x	; Write in LUT Memory
                lda long:bg_color_lut,x  
                sta long:TEXT_LUT_BG,x	; Write in LUT Memory                              
                inx
                cpx ##0x0040
                bne 1$
				rtl

;*********************************************************************
;************************** Init_Text_LUT ****************************
;*********************************************************************				
				
;  VICKY GAMMA TABLES
Init_Gamma		setas 		; Set 8bits
                setxl     ; Set Accumulator to 8bits
                ldx ##0000
1$  			LDA long:GAMMA_1_8_Tbl, x
                STA long:GAMMA_B_LUT_PTR, x
                STA long:GAMMA_G_LUT_PTR, x
                STA long:GAMMA_R_LUT_PTR, x
                inx
                cpx ##0x0100
                bne 1$
                rtl	

;*********************************************************************
;************************** Clear_Screen *****************************
;*********************************************************************
Clear_Screen:
				setas 
                setxl 
                ldx ##0000
                lda #0x20
1$:               
                sta long: TEXT_MEM,x
                inx 
                cpx ##0x12C0
                bne 1$
                rtl 

;*********************************************************************
;************************** Fill_Color   *****************************
;*********************************************************************
Fill_Color:      
                setas 
                setxl 
                ldx ##0000
                lda #0xE1
1$:              
                sta long: COLOR_MEM,x
                inx 
                cpx ##0x12C0
                bne 1$
                rtl
				
;*********************************************************************
;************************** Init FONT Mem*****************************
;*********************************************************************				
InitFONT_Mem:
                setas 
                setxl 
                ldx ##0000
1$:             lda long: fontset_std,x 
                sta long: VKY_IV_FONT_0,x
                inx 
                cpx ##0x0800
                bne 1$
                
                ; put JA font in font slot 2
                ldx ##0000
2$:             lda long: fontset_ja,x 
                sta long: VKY_IV_FONT_1,x
                inx 
                cpx ##0x0800
                bne 2$

				rtl

;*********************************************************************
;************************** Splash Screen ****************************
;*********************************************************************	
SplashText:      
                setas 
                setxl 
                ldx ##0x0000
PrintText                
                lda long: Text2Display,x 
                cmp #0x00
                beq EndSplash
                sta long: TEXT_MEM,x
                inx 
                bne PrintText
EndSplash                
                rtl			

;*********************************************************************
;************************** Init IRQ Srv *****************************
;*********************************************************************
;Init_IRQ_Service:
;
;				rti 
				
;*********************************************************************
;************************** Init NMI Srv *****************************
;*********************************************************************
Init_NMI_Service:

				rti 				


;	.section data_table ,data 	; An initialized data section in read/write memory (RAM).

;table: .byte 0x55
				.align 16
fg_color_lut:	.byte 0x00, 0x00, 0x00, 0xFF
                .byte 0x00, 0x00, 0x80, 0xFF
                .byte 0x00, 0x80, 0x00, 0xFF
                .byte 0x80, 0x00, 0x00, 0xFF
                .byte 0x00, 0x80, 0x80, 0xFF
                .byte 0x80, 0x80, 0x00, 0xFF
                .byte 0x80, 0x00, 0x80, 0xFF
                .byte 0x80, 0x80, 0x80, 0xFF
                .byte 0x00, 0x45, 0xFF, 0xFF
                .byte 0x13, 0x45, 0x8B, 0xFF
                .byte 0x00, 0x00, 0x20, 0xFF
                .byte 0x00, 0x20, 0x00, 0xFF
                .byte 0x20, 0x00, 0x00, 0xFF
                .byte 0x20, 0x20, 0x20, 0xFF
                .byte 0xFF, 0x80, 0x00, 0xFF
                .byte 0xFF, 0xFF, 0xFF, 0xFF

bg_color_lut:	.byte 0x00, 0x00, 0x00, 0xFF  ;BGRA
                .byte 0xAA, 0x00, 0x00, 0xFF
                .byte 0x00, 0x80, 0x00, 0xFF
                .byte 0x00, 0x00, 0x80, 0xFF
                .byte 0x00, 0x20, 0x20, 0xFF
                .byte 0x20, 0x20, 0x00, 0xFF
                .byte 0x20, 0x00, 0x20, 0xFF
                .byte 0x20, 0x20, 0x20, 0xFF
                .byte 0x1E, 0x69, 0xD2, 0xFF
                .byte 0x13, 0x45, 0x8B, 0xFF
                .byte 0x00, 0x00, 0x20, 0xFF
                .byte 0x00, 0x20, 0x00, 0xFF
                .byte 0x40, 0x00, 0x00, 0xFF
                .byte 0x10, 0x10, 0x10, 0xFF
                .byte 0x40, 0x40, 0x40, 0xFF
                .byte 0xFF, 0xFF, 0xFF, 0xFF
				
				.align 256
GAMMA_1_8_Tbl   .byte  0x00, 0x0b, 0x11, 0x15, 0x19, 0x1c, 0x1f, 0x22, 0x25, 0x27, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34
                .byte  0x36, 0x38, 0x3a, 0x3c, 0x3d, 0x3f, 0x41, 0x43, 0x44, 0x46, 0x47, 0x49, 0x4a, 0x4c, 0x4d, 0x4f
                .byte  0x50, 0x51, 0x53, 0x54, 0x55, 0x57, 0x58, 0x59, 0x5b, 0x5c, 0x5d, 0x5e, 0x60, 0x61, 0x62, 0x63
                .byte  0x64, 0x65, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75
                .byte  0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x84
                .byte  0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93
                .byte  0x94, 0x95, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0
                .byte  0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xac, 0xad
                .byte  0xad, 0xae, 0xaf, 0xb0, 0xb0, 0xb1, 0xb2, 0xb3, 0xb3, 0xb4, 0xb5, 0xb6, 0xb6, 0xb7, 0xb8, 0xb8
                .byte  0xb9, 0xba, 0xbb, 0xbb, 0xbc, 0xbd, 0xbd, 0xbe, 0xbf, 0xbf, 0xc0, 0xc1, 0xc2, 0xc2, 0xc3, 0xc4
                .byte  0xc4, 0xc5, 0xc6, 0xc6, 0xc7, 0xc8, 0xc8, 0xc9, 0xca, 0xca, 0xcb, 0xcc, 0xcc, 0xcd, 0xce, 0xce
                .byte  0xcf, 0xd0, 0xd0, 0xd1, 0xd2, 0xd2, 0xd3, 0xd4, 0xd4, 0xd5, 0xd6, 0xd6, 0xd7, 0xd7, 0xd8, 0xd9
                .byte  0xd9, 0xda, 0xdb, 0xdb, 0xdc, 0xdc, 0xdd, 0xde, 0xde, 0xdf, 0xe0, 0xe0, 0xe1, 0xe1, 0xe2, 0xe3
                .byte  0xe3, 0xe4, 0xe4, 0xe5, 0xe6, 0xe6, 0xe7, 0xe7, 0xe8, 0xe9, 0xe9, 0xea, 0xea, 0xeb, 0xec, 0xec
                .byte  0xed, 0xed, 0xee, 0xef, 0xef, 0xf0, 0xf0, 0xf1, 0xf1, 0xf2, 0xf3, 0xf3, 0xf4, 0xf4, 0xf5, 0xf5
                .byte  0xf6, 0xf7, 0xf7, 0xf8, 0xf8, 0xf9, 0xf9, 0xfa, 0xfb, 0xfb, 0xfc, 0xfc, 0xfd, 0xfd, 0xfe, 0xff				

fontset_std:		.incbin "font/std.fnt"
fontset_ja:			.incbin "font/kana.fnt"
;fontset_ansi:		.incbin "font/std-ansi.fnt"
;fontset_ibm_ansi:	.incbin "font/ibm-ansi.fnt"

				.align 16

Text2Display    .asciz " F256xE Kernal Development System..."	


;				.section BootVector ,text	;8bit Boot Vectors
; 65C816 Vector Layout 
Rsvd_Vct0		.word 	0x000000			; 0x00FFE0 - 0x00FFE1 
Rsvd_Vct1		.word 	0x000000			; 0x00FFE2 - 0x00FFE3 
COP816_Vector	.word 	0x000000 			; 0x00FFE4 - 0x00FFE5
BRK816_Vector	.word 	0x000000			; 0x00FFE6 - 0x00FFE7 
ABORT816_Vector	.word 	0x000000 			; 0x00FFE8 - 0x00FFE9
NMI816_Vector	.word 	Init_NMI_Service	; 0x00FFEA - 0x00FFEB
Rsvd_Vct2		.word 	0x000000			; 0x00FFEC - 0x00FFED
;IRQ816_Vector	.word 	Init_IRQ_Service 	; 0x00FFEE - 0x00FFEF
; Original 65C02 Vector Layout
Rsvd_Vct3		.word 	0x000000			; 0x00FFF0 - 0x00FFF1 
Rsvd_Vct4		.word 	0x000000			; 0x00FFF2 - 0x00FFF3 
COP02_Vector	.word 	0x000000 			; 0x00FFF4 - 0x00FFF5
Rsvd_Vct5		.word 	0x000000			; 0x00FFF6 - 0x00FFF7 
ABORT02_Vector	.word 	0x000000 			; 0x00FFF8 - 0x00FFF9
NMI02_Vector	.word 	Init_NMI_Service	; 0x00FFFA - 0x00FFFB
;RESET_Vector	.word 	__program_start 	; 0x00FFFC - 0x00FFFD
;IRQ02_Vector	.word 	Init_IRQ_Service 	; 0x00FFFE - 0x00FFFF

