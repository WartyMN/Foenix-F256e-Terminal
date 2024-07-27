;Internal Tiny VICKY Registers and Internal Memory Locations (LUTs)

GAMMA_B_LUT_PTR					.equlab		0xF00000
GAMMA_G_LUT_PTR					.equlab		0xF00400
GAMMA_R_LUT_PTR					.equlab		0xF00800
MOUSE_PTR_GRAP0					.equlab		0xF00C00

; IO Page 0
MASTER_CTRL_REG_L	    		.equlab 	0xF01000
;Control Bits Fields		
Mstr_Ctrl_Text_Mode_En  		.equlab 	0x01       ; Enable the Text Mode
Mstr_Ctrl_Text_Overlay  		.equlab 	0x02       ; Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
Mstr_Ctrl_Graph_Mode_En 		.equlab 	0x04       ; Enable the Graphic Mode
Mstr_Ctrl_Bitmap_En     		.equlab 	0x08       ; Enable the Bitmap Module In Vicky
Mstr_Ctrl_TileMap_En    		.equlab 	0x10       ; Enable the Tile Module in Vicky
Mstr_Ctrl_Sprite_En     		.equlab 	0x20       ; Enable the Sprite Module in Vicky
Mstr_Ctrl_GAMMA_En      		.equlab 	0x40       ; this Enable the GAMMA correction - The Analog and DVI have different color value, the GAMMA is great to correct the difference
Mstr_Ctrl_Disable_Vid   		.equlab 	0x80       ; This will disable the Scanning of the Video hence giving 100% bandwith to the CPU

MASTER_CTRL_REG_H	    		.equlab 	0xF01001
Mstr_Ctrl_Video_Mode    		.equlab 	0x01       ; 0 - 640x480@60Hz : 1 - 640x400@70hz (text mode) // 0 - 320x240@60hz : 1 - 320x200@70Hz (Graphic Mode & Text mode when Doubling = 1)
Mstr_Ctrl_Text_XDouble   		.equlab 	0x02       ; X Pixel Doubling
Mstr_Ctrl_Text_YDouble   		.equlab 	0x04       ; Y Pixel Doubling
Mstr_Ctrl_Turn_Sync_Off  		.equlab 	0x08      ; 1 = Turn off Sync
Mstr_Ctrl_Show_BG_InOverlay 	.equlab 	0x10   ; 1 = Allow the Background color to show up in Overlay mode
Mstr_Ctrl_FONT_Bank_Set 		.equlab 	0x20  ; 0 =(default) FONT Set 0, 1 = FONT Set 1
; Reserved - TBD
VKY_RESERVED_00         		.equlab 	0xF01002
VKY_RESERVED_01         		.equlab 	0xF01003
; 
BORDER_CTRL_REG         		.equlab 	0xF01004 ; Bit[0] - Enable (1 by default)  Bit[4..6]: X Scroll Offset ( Will scroll Left) (Acceptable Value: 0..7)
Border_Ctrl_Enable      		.equlab 	0x01
BORDER_COLOR_B          		.equlab 	0xF01005
BORDER_COLOR_G          		.equlab 	0xF01006
BORDER_COLOR_R          		.equlab 	0xF01007
BORDER_X_SIZE           		.equlab 	0xF01008; X-  Values: 0 - 32 (Default: 32)
BORDER_Y_SIZE           		.equlab 	0xF01009; Y- Values 0 -32 (Default: 32)
; Reserved - TBD
VKY_RESERVED_02         		.equlab 	0xF0100A
VKY_RESERVED_03         		.equlab 	0xF0100B
VKY_RESERVED_04         		.equlab 	0xF0100C
; Valid in Graphics Mode Only
BACKGROUND_COLOR_B      		.equlab 	0xF0100D ; When in Graphic Mode, if a pixel is "0" then the Background pixel is chosen
BACKGROUND_COLOR_G      		.equlab 	0xF0100E
BACKGROUND_COLOR_R      		.equlab 	0xF0100F ;
; Cursor Registers
VKY_TXT_CURSOR_CTRL_REG 		.equlab 	0xF01010   ;[0]  Enable Text Mode
Vky_Cursor_Enable       		.equlab 	0x01
Vky_Cursor_Flash_Rate0  		.equlab 	0x02
Vky_Cursor_Flash_Rate1  		.equlab 	0x04
VKY_TXT_START_ADD_PTR   		.equlab 	0xF01011   ; This is an offset to change the Starting address of the Text Mode Buffer (in x)
VKY_TXT_CURSOR_CHAR_REG 		.equlab 	0xF01012
VKY_TXT_CURSOR_COLR_REG 		.equlab 	0xF01013
VKY_TXT_CURSOR_X_REG_L  		.equlab 	0xF01014
VKY_TXT_CURSOR_X_REG_H  		.equlab 	0xF01015
VKY_TXT_CURSOR_Y_REG_L  		.equlab 	0xF01016
VKY_TXT_CURSOR_Y_REG_H  		.equlab 	0xF01017
; Line Interrupt 
VKY_LINE_IRQ_CTRL_REG   		.equlab 	0xF01018 ;[0] - Enable Line 0 - WRITE ONLY
VKY_LINE_CMP_VALUE_LO  			.equlab 	0xF01019 ;Write Only [7:0]
VKY_LINE_CMP_VALUE_HI  			.equlab 	0xF0101A ;Write Only [3:0]
VKY_PIXEL_X_POS_LO     			.equlab 	0xF01018 ; This is Where on the video line is the Pixel
VKY_PIXEL_X_POS_HI     			.equlab 	0xF01019 ; Or what pixel is being displayed when the register is read
VKY_LINE_Y_POS_LO      			.equlab 	0xF0101A ; This is the Line Value of the Raster
VKY_LINE_Y_POS_HI      			.equlab 	0xF0101B ; 
; Bitmap
;BM0
TyVKY_BM0_CTRL_REG       		.equlab 	0xF01100 
BM0_Ctrl                		.equlab 	0x01       ; Enable the BM0
BM0_LUT0                		.equlab 	0x02       ; LUT0
BM0_LUT1                		.equlab 	0x04       ; LUT1
TyVKY_BM0_START_ADDY_L   		.equlab 	0xF01101
TyVKY_BM0_START_ADDY_M   		.equlab 	0xF01102
TyVKY_BM0_START_ADDY_H   		.equlab 	0xF01103
;BM1
TyVKY_BM1_CTRL_REG       		.equlab 	0xF01108 
BM1_Ctrl                		.equlab 	0x01       ; Enable the BM0
BM1_LUT0                		.equlab 	0x02       ; LUT0
BM1_LUT1                		.equlab 	0x04       ; LUT1
TyVKY_BM1_START_ADDY_L   		.equlab 	0xF01109
TyVKY_BM1_START_ADDY_M   		.equlab 	0xF0110A
TyVKY_BM1_START_ADDY_H   		.equlab 	0xF0110B
;BM2
TyVKY_BM2_CTRL_REG       		.equlab 	0xF01110
BM2_Ctrl                		.equlab 	0x01       ; Enable the BM0
BM2_LUT0                		.equlab 	0x02       ; LUT0
BM2_LUT1                		.equlab 	0x04       ; LUT1
BM2_LUT2                		.equlab 	0x08       ; LUT2
TyVKY_BM2_START_ADDY_L   		.equlab 	0xF01111
TyVKY_BM2_START_ADDY_M   		.equlab 	0xF01112
TyVKY_BM2_START_ADDY_H   		.equlab 	0xF01113
; Tile Map 
TyVKY_TL_CTRL0          		.equlab 	0xF01200 
; Bit Field Definition for the Control Register
TILE_Enable             		.equlab 	0x01
TILE_LUT0               		.equlab 	0x02
TILE_LUT1               		.equlab 	0x04
TILE_LUT2               		.equlab 	0x08
TILE_SIZE               		.equlab 	0x10   ; 0 -> 16x16, 0 -> 8x8
;Tile MAP Layer 0 Registers
TL0_CONTROL_REG         		.equlab 	0xF01200       ; Bit[0] - Enable, Bit[3:1] - LUT Select,
TL0_START_ADDY_L        		.equlab 	0xF01201       ; Not USed right now - Starting Address to where is the MAP
TL0_START_ADDY_M        		.equlab 	0xF01202
TL0_START_ADDY_H        		.equlab 	0xF01203
TL0_MAP_X_SIZE_L        		.equlab 	0xF01204       ; The Size X of the Map
TL0_MAP_X_SIZE_H        		.equlab 	0xF01205
TL0_MAP_Y_SIZE_L        		.equlab 	0xF01206       ; The Size Y of the Map
TL0_MAP_Y_SIZE_H        		.equlab 	0xF01207
TL0_MAP_X_POS_L         		.equlab 	0xF01208       ; The Position X of the Map
TL0_MAP_X_POS_H         		.equlab 	0xF01209
TL0_MAP_Y_POS_L         		.equlab 	0xF0120A       ; The Position Y of the Map
TL0_MAP_Y_POS_H         		.equlab 	0xF0120B
;Tile MAP Layer 1 Registers
TL1_CONTROL_REG         		.equlab 	0xF0120C       ; Bit[0] - Enable, Bit[3:1] - LUT Select,
TL1_START_ADDY_L        		.equlab 	0xF0120D       ; Not USed right now - Starting Address to where is the MAP
TL1_START_ADDY_M        		.equlab 	0xF0120E
TL1_START_ADDY_H        		.equlab 	0xF0120F
TL1_MAP_X_SIZE_L        		.equlab 	0xF01210       ; The Size X of the Map
TL1_MAP_X_SIZE_H        		.equlab 	0xF01211
TL1_MAP_Y_SIZE_L        		.equlab 	0xF01212       ; The Size Y of the Map
TL1_MAP_Y_SIZE_H        		.equlab 	0xF01213
TL1_MAP_X_POS_L         		.equlab 	0xF01214       ; The Position X of the Map
TL1_MAP_X_POS_H         		.equlab 	0xF01215
TL1_MAP_Y_POS_L         		.equlab 	0xF01216       ; The Position Y of the Map
TL1_MAP_Y_POS_H         		.equlab 	0xF01217
;Tile MAP Layer 2 Registers
TL2_CONTROL_REG         		.equlab 	0xF01218       ; Bit[0] - Enable, Bit[3:1] - LUT Select,
TL2_START_ADDY_L        		.equlab 	0xF01219       ; Not USed right now - Starting Address to where is the MAP
TL2_START_ADDY_M        		.equlab 	0xF0121A
TL2_START_ADDY_H        		.equlab 	0xF0121B
TL2_MAP_X_SIZE_L        		.equlab 	0xF0121C       ; The Size X of the Map
TL2_MAP_X_SIZE_H        		.equlab 	0xF0121D
TL2_MAP_Y_SIZE_L        		.equlab 	0xF0121E       ; The Size Y of the Map
TL2_MAP_Y_SIZE_H        		.equlab 	0xF0121F
TL2_MAP_X_POS_L         		.equlab 	0xF01220       ; The Position X of the Map
TL2_MAP_X_POS_H         		.equlab 	0xF01221
TL2_MAP_Y_POS_L         		.equlab 	0xF01222       ; The Position Y of the Map
TL2_MAP_Y_POS_H         		.equlab 	0xF01223
; Tilemap Address
TILE_MAP_ADDY0_L      			.equlab 	0xF01280
TILE_MAP_ADDY0_M      			.equlab 	0xF01281
TILE_MAP_ADDY0_H      			.equlab 	0xF01282
TILE_MAP_ADDY0_CFG    			.equlab 	0xF01283 
TILE_MAP_ADDY1      			.equlab 	0xF01284
TILE_MAP_ADDY2      			.equlab 	0xF01288
TILE_MAP_ADDY3      			.equlab 	0xF0128C
TILE_MAP_ADDY4      			.equlab 	0xF01290
TILE_MAP_ADDY5      			.equlab 	0xF01294
TILE_MAP_ADDY6      			.equlab 	0xF01298
TILE_MAP_ADDY7      			.equlab 	0xF0129C
; Sprite Block0
SPRITE_Ctrl_Enable 				.equlab 	0x01
SPRITE_LUT0        				.equlab 	0x02
SPRITE_LUT1        				.equlab 	0x04
SPRITE_DEPTH0      				.equlab 	0x08    ; 00 = Total Front - 01 = In between L0 and L1, 10 = In between L1 and L2, 11 = Total Back
SPRITE_DEPTH1      				.equlab 	0x10
SPRITE_SIZE0       				.equlab 	0x20    ; 00 = 32x32 - 01 = 24x24 - 10 = 16x16 - 11 = 8x8
SPRITE_SIZE1       				.equlab 	0x40	

VICKY_PS2_INTERFACE_BASE       	.equlab 	0xF01007
VICKY_PS2_CTRL			       	.equlab 	0xF01640
VICKY_PS2_OUT			       	.equlab 	0xF01641
VICKY_PS2_KDB_IN		       	.equlab 	0xF01642
VICKY_PS2_MOUSE_IN		       	.equlab 	0xF01643
VICKY_PS2_STATUS		       	.equlab 	0xF01644

SP0_Ctrl           				.equlab 	0xF01900
SP0_Addy_L         				.equlab 	0xF01901
SP0_Addy_M         				.equlab 	0xF01902
SP0_Addy_H         				.equlab 	0xF01903
SP0_X_L            				.equlab 	0xF01904 
SP0_X_H            				.equlab 	0xF01905 
SP0_Y_L            				.equlab 	0xF01906  ; In the Jr, only the L is used (200 & 240)
SP0_Y_H            				.equlab 	0xF01907  ; Always Keep @ Zero '0' because in Vicky the value is still considered a 16bits value	

SP1_Ctrl           				.equlab 	0xF01908
SP1_Addy_L         				.equlab 	0xF01909
SP1_Addy_M         				.equlab 	0xF0190A
SP1_Addy_H         				.equlab 	0xF0190B
SP1_X_L            				.equlab 	0xF0190C 
SP1_X_H            				.equlab 	0xF0190D 
SP1_Y_L            				.equlab 	0xF0190E  ; In the Jr, only the L is used (200 & 240)
SP1_Y_H            				.equlab 	0xF0190F  ; Always Keep @ Zero '0' because in Vicky the value is still considered a 16bits value

SP2_Ctrl           				.equlab 	0xF01910
SP2_Addy_L         				.equlab 	0xF01911
SP2_Addy_M         				.equlab 	0xF01912
SP2_Addy_H         				.equlab 	0xF01913
SP2_X_L            				.equlab 	0xF01914 
SP2_X_H            				.equlab 	0xF01915 
SP2_Y_L            				.equlab 	0xF01916  ; In the Jr, only the L is used (200 & 240)
SP2_Y_H            				.equlab 	0xF01917  ; Always Keep @ Zero '0' because in Vicky the value is still considered a 16bits value	

SP3_Ctrl           				.equlab 	0xF01918
SP3_Addy_L         				.equlab 	0xF01919
SP3_Addy_M         				.equlab 	0xF0191A
SP3_Addy_H         				.equlab 	0xF0191B
SP3_X_L            				.equlab 	0xF0191C 
SP3_X_H            				.equlab 	0xF0191D 
SP3_Y_L            				.equlab 	0xF0191E  ; In the Jr, only the L is used (200 & 240)
SP3_Y_H            				.equlab 	0xF0191F  ; Always Keep @ Zero '0' because in Vicky the value is still considered a 16bits value

SP4_Ctrl           				.equlab 	0xF01920
SP4_Addy_L         				.equlab 	0xF01921
SP4_Addy_M         				.equlab 	0xF01922
SP4_Addy_H         				.equlab 	0xF01923
SP4_X_L            				.equlab 	0xF01924 
SP4_X_H            				.equlab 	0xF01925 
SP4_Y_L            				.equlab 	0xF01926  ; In the Jr, only the L is used (200 & 240)
SP4_Y_H            				.equlab 	0xF01927  ; Always Keep @ Zero '0' because in Vicky the value is still considered a 16bits value

; IO PAGE 0
TEXT_LUT_FG              		.equlab 	0xF01800
TEXT_LUT_BG              		.equlab 	0xF01840

;DMA
DMA_CTRL_REG        			.equlab 	0xF01F00
DMA_CTRL_Enable     			.equlab 	0x01
DMA_CTRL_1D_2D      			.equlab 	0x02
DMA_CTRL_Fill       			.equlab 	0x04
DMA_CTRL_Int_En     			.equlab 	0x08
DMA_CTRL_NotUsed0   			.equlab 	0x10
DMA_CTRL_NotUsed1   			.equlab 	0x20
DMA_CTRL_NotUsed2   			.equlab 	0x40
DMA_CTRL_Start_Trf  			.equlab 	0x80
DMA_DATA_2_WRITE    			.equlab 	0xF01F01 ; Write Only
DMA_STATUS_REG      			.equlab 	0xF01F01 ; Read Only
DMA_STATUS_TRF_IP   			.equlab 	0x80   ; Transfer in Progress
DMA_RESERVED_0      			.equlab 	0xF01F02 
DMA_RESERVED_1      			.equlab 	0xF01F03	
; Source Addy			
DMA_SOURCE_ADDY_L   			.equlab 	0xF01F04
DMA_SOURCE_ADDY_M   			.equlab 	0xF01F05
DMA_SOURCE_ADDY_H   			.equlab 	0xF01F06
DMA_RESERVED_2      			.equlab 	0xF01F07
; Destination Addy			
DMA_DEST_ADDY_L     			.equlab 	0xF01F08
DMA_DEST_ADDY_M     			.equlab 	0xF01F09
DMA_DEST_ADDY_H     			.equlab 	0xF01F0A
DMA_RESERVED_3      			.equlab 	0xF01F0B
; Size in 1D Mode			
DMA_SIZE_1D_L       			.equlab 	0xF01F0C
DMA_SIZE_1D_M       			.equlab 	0xF01F0D
DMA_SIZE_1D_H       			.equlab 	0xF01F0E
DMA_RESERVED_4      			.equlab 	0xF01F0F
; Size in 2D Mode			
DMA_SIZE_X_L        			.equlab 	0xF01F0C
DMA_SIZE_X_H        			.equlab 	0xF01F0D
DMA_SIZE_Y_L        			.equlab 	0xF01F0E
DMA_SIZE_Y_H        			.equlab 	0xF01F0F
; Stride in 2D Mode			
DMA_SRC_STRIDE_X_L  			.equlab 	0xF01F10
DMA_SRC_STRIDE_X_H  			.equlab 	0xF01F11
DMA_DST_STRIDE_Y_L  			.equlab 	0xF01F12
DMA_DST_STRIDE_Y_H  			.equlab 	0xF01F13
DMA_RESERVED_5      			.equlab 	0xF01F14
DMA_RESERVED_6      			.equlab 	0xF01F15
DMA_RESERVED_7      			.equlab 	0xF01F16
DMA_RESERVED_8      			.equlab 	0xF01F17
; PAGE 1
VKY_IV_FONT_0					.equlab 	0xF02000
VKY_IV_FONT_1					.equlab 	0xF02800

TyVKY_LUT0              		.equlab 	0xF03000 ; -$D000 - $D3FF
TyVKY_LUT1              		.equlab 	0xF03400 ; -$D400 - $D7FF
TyVKY_LUT2              		.equlab 	0xF03800 ; -$D800 - $DBFF
TyVKY_LUT3              		.equlab 	0xF03C00 ; -$DC00 - $DFFF
; Text Memory
TEXT_MEM              			.equlab 	0xF04000 	; IO Page 2
COLOR_MEM              			.equlab 	0xF06000 	; IO Page 3