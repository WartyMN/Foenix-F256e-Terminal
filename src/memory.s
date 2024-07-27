; native assembly code. see memory.h for the C interface.


;	.setcpu	"65C02"
	.rtmodel core,"65816"
	.rtmodel codemodel,"large"
	.rtmodel dataModel,"medium"
	.rtmodel version, "1"
;	.smart	on
;	.autoimport	on
;	.case	on
;	.debuginfo	off
;	.importzp	sp, sreg, regsave, regbank
;	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
;	.macpack	longbranch
	
; export to C
;	.public	_Memory_SwapInNewBank
;	.public	_Memory_RestorePreviousBank
;	.public _Memory_GetMappedBankNum
;	.public _Memory_Copy
#if _DMAA_
	.public Memory_CopyRectWithDMA
#endif
;	.public _Memory_CopyWithDMA
;	.public _Memory_FillWithDMA
;	.public _Memory_DebugOut

; ZP_LK exports:

;	.exportzp	zp_bank_slot
;	.exportzp	zp_bank_num
;	.exportzp	zp_old_bank_num
	.public		zp_to_addr
	.public		zp_from_addr
	.public		zp_copy_len
;	.exportzp	zp_x
;	.exportzp	zp_y
;	.exportzp	zp_screen_id
	.public		zp_phys_addr_lo
	.public		zp_phys_addr_med
	.public		zp_phys_addr_hi
	.public		zp_cpu_addr_lo
	.public		zp_cpu_addr_hi
;	.public		zp_search_loc_byte
;	.public		zp_search_loc_page
;	.public		zp_search_loc_bank
	.public		zp_width_lo
	.public		zp_width_hi
	.public		zp_height_lo
	.public		zp_height_hi
	.public		zp_src_stride_lo
	.public		zp_src_stride_hi
	.public		zp_dst_stride_lo
	.public		zp_dst_stride_hi
	.public		zp_temp_1
	.public		zp_other_byte

	

; F256 DMA addresses and bit values

DMA_CTRL			.equlab	0xf01f00	; DMA Control Register
DMA_CTRL_START		.equ	0x80		; Start the DMA operation
DMA_CTRL_FILL		.equ	0x04		; Do a FILL operation (if off, will do COPY)
DMA_CTRL_2D			.equ	0x02		; Use 2D copy/fill
DMA_CTRL_ENABLE		.equ	0x01		; Enable the DMA engine

DMA_STATUS			.equlab	0xf01f01	; DMA status register (Read Only)
DMA_STAT_BUSY		.equ	0x80		; DMA engine is busy with an operation

DMA_FILL_VAL		.equlab	0xf01f01	; Byte value to use for fill operations
DMA_SRC_ADDR		.equlab	0xf01f04	; Source address (system bus - 3 byte)
DMA_DST_ADDR		.equlab	0xf01f08	; Destination address (system bus - 3 byte)
DMA_COUNT			.equlab	0xf01f0c	; Number of bytes to fill
DMA_WIDTH			.equlab	0xf01f0c	; Width of 2D operation - 16 bits - only available when 2D is set
DMA_HEIGHT			.equlab	0xf01f0e	; Height of 2D operation - 16 bits - only available when 2D is set
DMA_SRC_STRIDE		.equlab	0xf01f10	; Source stride for 2D operation - 16 bits - only available when 2D COPY is set
DMA_DST_STRIDE		.equlab	0xf01f12	; Destination stride for 2D operation - 16 bits - only available when 2D is set


; -- ZEROPAGE_LK starts at $10

	.section ztiny,bss
zp_to_addr:				.space 3	; $0
zp_from_addr:			.space 3
zp_copy_len:			.space 3
zp_phys_addr_lo:		.space 1
zp_phys_addr_med:		.space 1
zp_phys_addr_hi:		.space 1
zp_cpu_addr_lo:			.space 1
zp_cpu_addr_hi:			.space 1	
zp_search_loc_byte:		.space 1
zp_search_loc_page:		.space 1
zp_search_loc_bank:		.space 1	; $10
zp_width_lo:			.space 1
zp_width_hi:			.space 1
zp_height_lo:			.space 1
zp_height_hi:			.space 1
zp_src_stride_lo:		.space 1
zp_src_stride_hi:		.space 1
zp_dst_stride_lo:		.space 1
zp_dst_stride_hi:		.space 1
zp_temp_1:				.space 1
zp_other_byte:			.space 1
zp_x:					.space 2
zp_y:					.space 2	; $d and $e

	
	.section farcode, text
	



; ---------------------------------------------------------------
; void __fastcall__ Memory_DebugOut(void)
; ---------------------------------------------------------------
;// call to a routine in memory.asm that writes an illegal opcode followed by address of debug buffer
;// that is a simple to the f256jr emulator to write the string at the debug buffer out to the console

;.segment	"CODE"
;
;.proc	_Memory_DebugOut: near
;
;.segment	"CODE"
;
;	.byte $FC				; illegal opcode that to Paul's JR emulator means "next 2 bytes are address of a string I should write to console"
;	.byte $00;
;	.byte $03;				; we're using $0300 hard coded as a location for the moment.
;
;	RTS
;
;.endproc




; ---------------------------------------------------------------
; void __fastcall__ Memory_Copy(void)
; ---------------------------------------------------------------
;// call to a routine in memory.asm that copies specified number of bytes from src to dst
;// set zp_to_addr, zp_from_addr, zp_copy_len before calling.
;// credit: http://6502.org/source/general/memory_move.html


;.segment	"OVERLAY_NOTICE_BOARD"
;
;.proc	_Memory_Copy: near
;
;.segment	"OVERLAY_NOTICE_BOARD"
;
;MOVEUP:  LDX _zp_copy_len		; the last byte must be moved first
;         CLC         			; start at the final pages of FROM and TO
;         TXA
;         ADC _zp_from_addr+1
;         STA _zp_from_addr+1
;         CLC
;         TXA
;         ADC _zp_to_addr+1
;         STA _zp_to_addr+1
;         INX         			; allows the use of BNE after the DEX below
;         LDY _zp_copy_len+1
;         BEQ MU3
;         DEY          			; move bytes on the last page first
;         BEQ MU2
;MU1:     LDA (_zp_from_addr),Y
;         STA (_zp_to_addr),Y
;         DEY
;         BNE MU1
;MU2:     LDA (_zp_from_addr),Y 	; handle Y = 0 separately
;         STA (_zp_to_addr),Y
;MU3:     DEY
;         DEC _zp_from_addr+1   	; move the next page (if any)
;         DEC _zp_to_addr+1
;         DEX
;         BNE MU1
;         RTS
;
;.endproc




; ---------------------------------------------------------------
; void __fastcall__ Memory_CopyRectWithDMA(void)
; ---------------------------------------------------------------
;// call to a routine in memory.asm that copies a specific "rectangle" of data from one buffer to another (think blit an image to a bitmap)
;// set zp_to_addr, zp_from_addr, zp_width, zp_height, zp_src_stride, zp_dst_stride before calling.
;// this version uses the F256's DMA capabilities to copy, so addresses can be 24 bit (system memory, not CPU memory)
;// in other words, no need to page either dst or src into CPU space

; status - 2024-07-03: this function compiles, but has not been debugged, and generally results in freezes. DMA appears to be working in C with F256Ke load.

#if _DMAA_

Memory_CopyRectWithDMA:

			SEI						; disable interrupts

			LDA #0
			STA long: DMA_CTRL		; Turn off the DMA engine

			; Enable the DMA engine and set it up for a (2D) copy operation:
			LDA #DMA_CTRL_ENABLE | DMA_CTRL_2D
			STA long: DMA_CTRL

			;Source address (3 byte):
			LDA zp_from_addr
			STA long: DMA_SRC_ADDR
			LDA zp_from_addr+1
			STA long: DMA_SRC_ADDR+1
			LDA zp_from_addr+2
			;AND #0x07
			STA long: DMA_SRC_ADDR+2

			;Destination address (3 byte):
			LDA zp_to_addr
			STA long: DMA_DST_ADDR
			LDA zp_to_addr+1
			STA long: DMA_DST_ADDR+1
			LDA zp_to_addr+2
			;AND #0x07
			STA long:DMA_DST_ADDR+2

			; source copy width
			LDA zp_width_lo
			STA long: DMA_WIDTH
			LDA zp_width_hi
			STA long: DMA_WIDTH+1

			; source copy height
			LDA zp_height_lo
			STA long: DMA_HEIGHT
			LDA zp_height_hi
			STA long: DMA_HEIGHT+1

			; source stride
			LDA zp_src_stride_lo
			STA long: DMA_SRC_STRIDE
			LDA zp_src_stride_hi
			STA long: DMA_SRC_STRIDE+1

			; dest stride
			LDA zp_dst_stride_lo
			STA long: DMA_DST_STRIDE
			LDA zp_dst_stride_hi
			STA long: DMA_DST_STRIDE+1

			; flip the START flag to trigger the DMA operation
			LDA long: DMA_CTRL
			ORA #DMA_CTRL_START
			STA long: DMA_CTRL
			; wait for it to finish

wait_dma:	LDA long: DMA_STATUS
			BMI wait_dma            ; Wait until DMA is not busy 
			
			LDA #0
			STA long: DMA_CTRL		; Turn off the DMA engine
			
			CLI						; re-enable interrupts
			
			RTS
;.endproc

#endif

; ---------------------------------------------------------------
; void __fastcall__ Memory_CopyWithDMA(void)
; ---------------------------------------------------------------
;// call to a routine in memory.asm that copies specified number of bytes from src to dst
;// set zp_to_addr, zp_from_addr, zp_copy_len before calling.
;// this version uses the F256's DMA capabilities to copy, so addresses can be 24 bit (system memory, not CPU memory)
;// in other words, no need to page either dst or src into CPU space

; status - 2024-03-17: DMA works (1 out of 5 or so times), but very unstable. others report same instability. commenting out until a more stable way can be identified. 


;.segment	"CODE"
;
;.proc	_Memory_CopyWithDMA: near
;
;.segment	"CODE"
;
;			SEI					; disable interrupts
;
;			; Wait for VBlank period
;LINE_NO = 261*2  ; 240+21
;        	LDA #<LINE_NO
;        	LDX #>LINE_NO
;wait1:
;        	CPX $D01B
;       	 	BEQ wait1
;wait2:
;        	cmp $D01A
;        	CMP wait2
;
;wait3:
;        	CPX $D01B
;        	BNE wait3
;wait4:
;        	CMP $D01A
;        	BNE wait4
;
;
;			STZ DMA_CTRL			; Turn off the DMA engine
;
;			NOP						; random experimenting with trying to prevent timing issue
;			NOP
;			NOP
;			NOP
;			NOP
;			
;			; Enable the DMA engine and set it up for a (1D) copy operation:
;			LDA #DMA_CTRL_ENABLE
;			STA DMA_CTRL
;
;			NOP						; random experimenting with trying to prevent timing issue
;			NOP
;			NOP
;			NOP
;			NOP
;			
;			;Source address (3 byte):
;			LDA _zp_from_addr
;			STA DMA_SRC_ADDR
;			LDA _zp_from_addr+1
;			STA DMA_SRC_ADDR+1
;			LDA _zp_from_addr+2
;			AND #$07
;			STA DMA_SRC_ADDR+2
;
;			;Destination address (3 byte):
;			LDA _zp_to_addr
;			STA DMA_DST_ADDR
;			LDA _zp_to_addr+1
;			STA DMA_DST_ADDR+1
;			LDA _zp_to_addr+2
;			AND #$07
;			STA DMA_DST_ADDR+2
;
;			; Num bytes to copy
;			LDA _zp_copy_len
;			STA DMA_COUNT
;			LDA _zp_copy_len+1
;			STA DMA_COUNT+1
;			LDA _zp_copy_len+2
;			STA DMA_COUNT+2
;
;			; flip the START flag to trigger the DMA operation
;			LDA DMA_CTRL
;			ORA #DMA_CTRL_START
;			STA DMA_CTRL
;			; wait for it to finish
;
;wait_dma:	LDA DMA_STATUS
;			BMI wait_dma            ; Wait until DMA is not busy 
;			
;			NOP
;			NOP
;			NOP
;			NOP
;			NOP
;
;			STZ DMA_CTRL			; Turn off the DMA engine
;			
;			NOP
;			NOP
;			NOP
;			NOP
;			NOP
;			
;			CLI						; re-enable interrupts
;			
;			RTS
;.endproc


; ---------------------------------------------------------------
; void __fastcall__ Memory_FillWithDMA(void)
; ---------------------------------------------------------------
;// call to a routine in memory.asm that fills the specified number of bytes to the dst
;// set zp_to_addr, zp_copy_len to num bytes to fill, and zp_other_byte to the fill value before calling.
;// this version uses the F256's DMA capabilities to fill, so addresses can be 24 bit (system memory, not CPU memory)
;// in other words, no need to page either dst into CPU space


;.segment	"CODE"
;
;.proc	_Memory_FillWithDMA: near
;
;.segment	"CODE"
;
;			SEI					; disable interrupts
;
;LINE_NO = 261*2  ; 240+21
;        lda #<LINE_NO
;        ldx #>LINE_NO
;wait1:
;        cpx $D01B
;        beq wait1
;wait2:
;        cmp $D01A
;        beq wait2
;
;wait3:
;        cpx $D01B
;        bne wait3
;wait4:
;        cmp $D01A
;        bne wait4
;
;			STZ DMA_CTRL			; Turn off the DMA engine
;			
;			; Enable the DMA engine and set it up for a FILL operation:
;			LDA #DMA_CTRL_FILL | DMA_CTRL_ENABLE
;			STA DMA_CTRL
;
;			; the fill value
;            lda _zp_other_byte
;            sta DMA_FILL_VAL
;            
;			;Destination address (3 byte):
;			LDA _zp_to_addr
;			STA DMA_DST_ADDR
;			LDA _zp_to_addr+1
;			STA DMA_DST_ADDR+1
;			LDA _zp_to_addr+2
;			AND #$07
;			STA DMA_DST_ADDR+2
;
;			; Num bytes to fill
;			LDA _zp_copy_len
;			STA DMA_COUNT
;			LDA _zp_copy_len+1
;			STA DMA_COUNT+1
;			LDA _zp_copy_len+2
;			STA DMA_COUNT+2
;
;			; flip the START flag to trigger the DMA operation
;			LDA DMA_CTRL
;			ORA #DMA_CTRL_START
;			STA DMA_CTRL
;			; wait for it to finish
;
;wait_dma:	LDA DMA_STATUS
;			BMI wait_dma            ; Wait until DMA is not busy 
;			
;			NOP
;			NOP
;			NOP
;			NOP
;			NOP
;			NOP
;			
;			NOP
;			NOP
;
;			NOP
;			NOP
;			
;			CLI						; re-enable interrupts
;			
;			RTS
;.endproc



