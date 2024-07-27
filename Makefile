VPATH = src

# will build for F256K2e with "make"
# "make" OR "make M=_F256K2_" OR "make M=_F256K_" OR "make M=_F256JR_"
# also include "D=_DMAA_" or "D=_DMAC_" or "D=_NO_DMA_" (any value other than "_DMAC_" or "_DMAA_" will block use of DMA)

#FOENIX = module/Calypsi-65816-Foenix

# debug logging levels: 1=error, 2=warn, 3=info, 4=debug general, 5=allocations
# DEBUG_DEF_1=LOG_LEVEL_1
# DEBUG_DEF_2=LOG_LEVEL_2
# DEBUG_DEF_3=LOG_LEVEL_3
# DEBUG_DEF_4=LOG_LEVEL_4
# DEBUG_DEF_5=LOG_LEVEL_5
DEBUG_DEF_1=NO_LOG_LEVEL_1
DEBUG_DEF_2=NO_LOG_LEVEL_2
DEBUG_DEF_3=NO_LOG_LEVEL_3
DEBUG_DEF_4=NO_LOG_LEVEL_4
DEBUG_DEF_5=NO_LOG_LEVEL_5

# whether disk or serial debug will be used, IF debug is actually on
# defining serial debug means serial will be used, not defining it means disk will be used. 
DEBUG_VIA_SERIAL=USE_SERIAL_LOGGING
#DEBUG_VIA_SERIAL=USE_DISK_LOGGING

# directories
BINDIR := bin

# Common source files
ASM_SRCS = f256xe_startup.s memory.s
C_SRCS = app.c bitmap.c comm_buffer.c debug.c dma.c event.c general.c kernel.c keyboard.c list.c screen.c serial.c startup.c strings.c sys.c text.c ff.c f256xe_diskio.c ffunicode.c

MODEL = --code-model=large --data-model=medium
LIB_MODEL = lc-md

#FOENIX_LINKER_RULES = linker-files/f256-plain.scm
FOENIX_LINKER_RULES = linker-files/f256-term.scm

# Object files
OBJS = $(ASM_SRCS:%.s=obj/%.o) $(C_SRCS:%.c=obj/%.o)
OBJS_DEBUG = $(ASM_SRCS:%.s=obj/%-debug.o) $(C_SRCS:%.c=obj/%-debug.o)

obj/%.o: %.s
	as65816 --core=65816 $(MODEL) --target=Foenix --list-file=$(@:%.o=%.lst) -Iinclude -o $@ $<

obj/%.o: %.c
	cc65816 --core=65816 $(MODEL) -D$(M)=1 -D$(D)=1 -D$(DEBUG_DEF_1) -D$(DEBUG_DEF_2) -D$(DEBUG_DEF_3) -D$(DEBUG_DEF_4) -D$(DEBUG_DEF_5) -D$(DEBUG_VIA_SERIAL) --list-file=$(@:%.o=%.lst) -Iinclude -o $@ $<

obj/%-debug.o: %.s
	as65816 --core=65816 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -Iinclude -o $@ $<

obj/%-debug.o: %.c
	cc65816 --core=65816 $(MODEL) --debug --list-file=$(@:%.o=%.lst) -Iinclude -o $@ $<

fterm.pgz:  $(OBJS)
	ln65816 -o $(BINDIR)/$@ $^ $(FOENIX_LINKER_RULES) clib-$(LIB_MODEL).a --output-format=pgz --list-file=obj/_fterm.lst --cross-reference --rtattr printf=medium --rtattr cstartup=Foenix --heap-size=16384

## ftp it to the linux box
#	~/dev/bbedit-workspace-foenix/f256k2-FileManager/_ftp_to_linux.sh ~/dev/bbedit-workspace-foenix/f256k2-FileManager/bin/fterm.pgz

## upload it to the foenix
	python3 $(FOENIXMGR)/FoenixMgr/fnxmgr.py --boot RAM
	python3 $(FOENIXMGR)/FoenixMgr/fnxmgr.py --run-pgz $(BINDIR)/fterm.pgz
	
clean:
	-rm $(OBJS) $(OBJS:%.o=%.lst) $(OBJS_DEBUG) $(OBJS_DEBUG:%.o=%.lst)
	-rm bin/fterm.pgz fterm-debug.lst fterm-Foenix.lst
