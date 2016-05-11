CPP_FILES := $(wildcard *.cpp)
ASM_FILES := $(wildcard *.S)
OBJ_FILES := $(CPP_FILES:.cpp=.o) $(ASM_FILES:.S=.o)
CC_FLAGS := -std=c++11 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Wno-unused-parameter -Wno-uninitialized
LD_FLAGS := -T linker.ld -ffreestanding -fno-use-linker-plugin -O2 -nostdlib

all: KernelThunk.hpp ../nightbeliever.krnl

%.o: %.cpp
	i686-elf-g++ $(CC_FLAGS) -c -o $@ $<

%.o: %.S
	i686-elf-as -o $@ $<

../nightbeliever.krnl: $(OBJ_FILES) linker.ld
	i686-elf-g++ $(LD_FLAGS) -o ../nightbeliever.krnl $(OBJ_FILES)

KernelThunk.hpp: thunkgen.py Kernel.hpp
	python thunkgen.py

clean:
	rm *.o || true
	rm ../nightbeliever.krnl || true
	rm KernelThunk.cpp KernelThunk.hpp || true
