CROSS_PREFIX ?= riscv64-linux-gnu-

ASFLAGS += -march=rv64imac -mabi=lp64

CFLAGS += -march=rv64imac -mabi=lp64

includes += -Iarch/riscv/include

objs += arch/riscv/boot.o
objs += drivers/uart/8250.o

include lib/libfdt/Makefile

all: kernel.bin

kernel.bin: kernel.elf
	$(E) "  OBJCOPY " $@
	$(Q) $(CROSS_PREFIX)objcopy -O binary kernel.elf kernel.bin

archclean:
	$(Q) rm -f kernel.bin

.PHONY: archclean
