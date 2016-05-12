CPP_FILES := $(wildcard *.cpp)
ASM_FILES := $(wildcard *.S)
OBJ_FILES := $(ASM_FILES:.S=.o) $(CPP_FILES:.cpp=.o)
TARGET_FLAGS := -m32 -target i686-apple-darwin-macho
CC_FLAGS := -std=c++11 -ffreestanding -c -nostdlib -nodefaultlibs -O2 -Wall -Wextra -fno-exceptions -fno-rtti $(TARGET_FLAGS)
AS_FLAGS := -c $(TARGET_FLAGS)
LD_FLAGS := -v $(TARGET_FLAGS) -ffreestanding -nostdlib -nodefaultlibs -Wl,-image_base,0xC0000000

all: KernelThunk.hpp ../nightbeliever.krnl

%.o: %.cpp
	clang $(CC_FLAGS) -c -o $@ $<

%.o: %.S
	clang $(AS_FLAGS) -o $@ $<

../nightbeliever.krnl: $(OBJ_FILES) linker.ld
	clang $(LD_FLAGS) -o ../nightbeliever.krnl $(OBJ_FILES)

KernelThunk.hpp: thunkgen.py Kernel.hpp
	python thunkgen.py

clean:
	rm *.o || true
	rm ../nightbeliever.krnl || true
	rm KernelThunk.cpp KernelThunk.hpp || true
