;;;
;;; Register address definitions for the math coprocessor
;;;

; Unsigned Multiplier In A (16Bits), In B (16Bits), Answer (32Bits)
;Unsigned Multiplier 16*16
UNSIGNED_MULT_A_LO  .equlab 	0xF01E00
UNSIGNED_MULT_A_HI  .equlab 	0xF01E01
UNSIGNED_MULT_B_LO  .equlab 	0xF01E02
UNSIGNED_MULT_B_HI  .equlab 	0xF01E03
;Unsigned Divider 16/16
UNSIGNED_DIV_DEM_LO .equlab 	0xF01E04    ;Denominator
UNSIGNED_DIV_DEM_HI .equlab 	0xF01E05
UNSIGNED_DIV_NUM_LO .equlab 	0xF01E06    ;Numerator
UNSIGNED_DIV_NUM_HI .equlab 	0xF01E07
; Unsigned Multiplier In A  	0x6Bits), In B (16Bits), Answer (32Bits)
ADDER32_A_LL        .equlab 	0xF01E08
ADDER32_A_LH        .equlab 	0xF01E09
ADDER32_A_HL        .equlab 	0xF01E0A
ADDER32_A_HH        .equlab 	0xF01E0B
ADDER32_B_LL        .equlab 	0xF01E0C
ADDER32_B_LH        .equlab 	0xF01E0D
ADDER32_B_HL        .equlab 	0xF01E0E
ADDER32_B_HH        .equlab 	0xF01E0F
; Results Unsigned Mult
UNSIGNED_MULT_AL_LO .equlab 	0xF01E10
UNSIGNED_MULT_AL_HI .equlab 	0xF01E11
UNSIGNED_MULT_AH_LO .equlab 	0xF01E12
UNSIGNED_MULT_AH_HI .equlab 	0xF01E13
; Results Unsigned Div
UNSIGNED_DIV_QUO_LO .equlab 	0xF01E14
UNSIGNED_DIV_QUO_HI .equlab 	0xF01E15
UNSIGNED_DIV_REM_LO .equlab 	0xF01E16
UNSIGNED_DIV_REM_HI .equlab 	0xF01E17
; Results ADD32
ADDER32_R_LL        .equlab 	0xF01E18
ADDER32_R_LH        .equlab 	0xF01E19
ADDER32_R_HL        .equlab 	0xF01E1A
ADDER32_R_HH        .equlab 	0xF01E1B