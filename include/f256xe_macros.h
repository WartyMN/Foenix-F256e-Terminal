; Macro 
;M	#0x20	00100000	Accumulator register size (native mode only), (0 = 16-bit, 1 = 8-bit)
;X	#0x10	00010000	Index register size (native mode only), (0 = 16-bit, 1 = 8-bit)
setaxl          .macro
					rep #0x30        ; set A&X long 
                .endm
				
setas           .macro
					sep #0x20        ; set A short 
                .endm				
	
setal           .macro
					rep #0x20        ; set A long 
                .endm	
				
; Set 8 bit index registers               
setxs           .macro
					sep #0x10        ; set X short 
                .endm

; Set 16-bit index registers
setxl           .macro
					rep #0x10        ; set X long 
                .endm

; Set the direct page. 
; Note: This uses the accumulator and leaves A set to 16 bits. 
setdp           .macro dp             
                pha             ; begin setdp macro 
                php
                setal 
                lda ##\dp         ; set DP to page 0
                tcd             
                plp
                pla             ; end setdp macro 
                .endm 

setdbr          .macro dbr      ; Set the B (Data bank) register 
                PHA             ; begin setdbr macro 
                PHP
                setas
                LDA #\dbr
                PHA
                PLB
                PLP
                PLA             ; end setdbr macro 
                .endm                 