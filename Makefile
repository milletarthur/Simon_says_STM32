### Do not modify: edit Makefile.project instead ###

include Makefile.project

#LDSCRIPT = link/stm32l073.ld
LDSCRIPT = link/stm32f446.ld

LDDEPS = $(LDSCRIPT) link/stm32_generic.ld

TOOLSDIR = devtools

SRCDIR = src
OBJDIR = obj
BINDIR = bin

ELF = $(BINDIR)/$(PROGNAME).elf
BIN = $(BINDIR)/$(PROGNAME).bin

OBJS = $(patsubst $(SRCDIR)/%.S,$(OBJDIR)/%.o,$(ASMS))
OBJS += $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
OBJS += $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPPSRCS))

CRCOBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CRCSRC))
CRCOBJOK = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%_ok.o,$(CRCSRC))
CRCLSTOK = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%_ok.lst,$(CRCSRC))
CRCOBJS = $(filter-out $(CRCOBJ),$(OBJS)) $(CRCOBJOK)

ARCHFLAGS = -mtune=cortex-m4 -mthumb -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16
FFLAGS = -fsingle-precision-constant
DIRFLAGS += -Isrc
CFLAGS = $(ARCHFLAGS) $(FFLAGS) -O2 -Os -Wall -Werror -Wdouble-promotion -std=gnu99 $(DIRFLAGS)
CFLAGS += -Wno-error=unused-variable -Wno-error=unused-function

VERBOSE ?= quiet
VERBOSE_quiet=0
VERBOSE_normal=1
VERBOSE_verbose=2
VERBOSE_all=3
V=$(VERBOSE_$(VERBOSE))

# commandes principales
CMD=$(CMD_$V)
CMD_0=@

# commandes annexes
CMD2=$(CMD2_$V)
CMD2_0=@
CMD1_1=@

# affichage de l'étape
DISP=$(DISP_$V)
DISP_0=@
DISP_1=@: 
DISP_2=@: 

all: $(ELF)
	@echo "================================================================"
	@echo "Compilation done"
	@echo "You can use "$(MAKE) V=x ..." to change verbosity (x=0, 1, 2, 3)"
	@echo "================================================================"

bin: $(BIN)

clean :
	rm -rf $(OBJDIR) $(BINDIR)

$(OBJDIR)/%.o : $(SRCDIR)/%.c $(CDEPS) Makefile
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "COMPILE $< => $@"
	$(CMD)arm-none-eabi-gcc $(CFLAGS) -Wa,-ahlns=$(OBJDIR)/$*.lst -g -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(CPPDEPS) $(CDEPS) Makefile
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "COMPILE $< => $@"
	$(CMD)arm-none-eabi-g++ $(CFLAGS) -Wa,-ahlns=$(OBJDIR)/$*.lst -g -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.S $(ASMDEPS) Makefile
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "COMPILE $< => $@"
	$(CMD)arm-none-eabi-gcc $(ARCHFLAGS) -Wa,-ahlns=$(OBJDIR)/$*.lst -g -c $< -o $@

$(ELF).nocrc : $(OBJS) $(LDDEPS)
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "LINK (without crc) $(OBJS) => $@"
	$(CMD)arm-none-eabi-$(LINKER) $(ARCHFLAGS) $(OBJS) $(LINKFLAGS) -T$(LDSCRIPT) -o $@

$(ELF) : $(CRCOBJS)
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo -n "LINK $(CRCOBJS) => $@\n"
	$(CMD)arm-none-eabi-$(LINKER) $(ARCHFLAGS) $(CRCOBJS) $(LINKFLAGS) -T$(LDSCRIPT) -o $@

$(BIN).nocrc : $(ELF).nocrc
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "CREATE (without crc) $< => $@"
	$(CMD)arm-none-eabi-objcopy -R .crc_info -O binary $< $@

$(BIN) : $(ELF)
	$(CMD2)mkdir -p $(dir $@)
	$(DISP)echo "CREATE $< => $@"
	$(CMD)arm-none-eabi-objcopy -O binary $< $@

$(CRCOBJOK) : $(BIN).nocrc $(CRCSRC)
	$(CMD2)chmod +x $(TOOLSDIR)/stm32-crc
	$(CMD)echo -n "COMPUTE CRC : ";\
	 CRC_LINE="$(shell $(TOOLSDIR)/stm32-crc $<)";\
	 echo -n "$$CRC_LINE\n";\
	 echo -n "RECOMPILE (with crc) $(CRCSRC) => $(CRCOBJOK)\n";\
	 arm-none-eabi-gcc $(CFLAGS) -Wa,-ahlns=$(CRCLSTOK) -g -c $(CRCSRC) -o $(CRCOBJOK) \
	   -DFLASH_CRC=0x`echo "$$CRC_LINE" | awk '{print $$1}'` \
	   -DFLASH_SIZE=` echo "$$CRC_LINE" | awk '{print $$2}'`

dump : $(ELF)
	$(DISP)echo "DUMP $<"
	$(CMD)arm-none-eabi-objdump -h -t -d $(ELF)

load: $(ELF)
	$(CMD2)chmod +x $(TOOLSDIR)/stm32-load
	$(DISP)echo "LOAD $<"
	$(CMD)$(TOOLSDIR)/stm32-load $<

debug: $(ELF) load
	$(CMD2)chmod +x $(TOOLSDIR)/stm32-gdb
	$(CMD2)chmod +x $(TOOLSDIR)/stm32-debug
	$(DISP)echo "DEBUG $<"
	$(CMD)$(TOOLSDIR)/stm32-debug $<

rawdebug: $(ELF) load
	$(CMD2)chmod +x $(TOOLSDIR)/stm32-gdb
	$(DISP)echo "DEBUG $<"
	$(CMD)$(TOOLSDIR)/stm32-gdb $<

