INCLUDE_DIRS = ../util/
COMPILER_DIR = C:/SysGCC/risc-v/bin
CC = $(COMPILER_DIR)/riscv64-unknown-elf-gcc
RV_CFG = -march=rv32im -mabi=ilp32
CFLAGS = -c $(RV_CFG) -I $(INCLUDE_DIRS) -O0
LDFLAGS = -nostartfiles -specs=nano.specs -ffunction-sections $(RV_CFG) -T ../linker.ld

OBJS = *.o
SRC_FILES = *.c $(INCLUDE_DIRS)/*.c

all: $(OBJS) final.elf
	$(COMPILER_DIR)/riscv64-unknown-elf-objcopy -O binary final.elf final.bin
	rm -rf *.o

final.elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJS): $(SRC_FILES) 
	$(CC) $(CFLAGS) $^ 

.PHONY: clean dump size bin

clean:
	rm -f *.o *.bin *.elf

dump:
	$(COMPILER_DIR)/riscv64-unknown-elf-objdump -D final.elf

size:
	$(COMPILER_DIR)/riscv64-unknown-elf-size -A final.elf


