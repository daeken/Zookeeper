CPP_FILES := $(wildcard *.cpp) $(wildcard XboxKernel/*.cpp)
ASM_FILES := $(wildcard *.s)
OBJ_FILES := $(ASM_FILES:.s=.o) $(CPP_FILES:.cpp=.o)
TARGET_FLAGS := -m32 -target i686-apple-darwin-macho
CC_FLAGS := -std=c++11 -I. -ffreestanding -c -nostdlib -O2 -fno-exceptions -fno-rtti -Wno-implicit-exception-spec-mismatch $(TARGET_FLAGS)
AS_FLAGS := -c $(TARGET_FLAGS)
LD_FLAGS := $(TARGET_FLAGS) -ffreestanding -nostdlib -Wl,-image_base,0xC0000000

all: XboxKernel/KernelThunk.hpp ../nightbeliever.krnl

%.o: %.cpp
	clang $(CC_FLAGS) -c -o $@ $<

%.o: %.s
	clang $(AS_FLAGS) -o $@ $<

../nightbeliever.krnl: $(OBJ_FILES) linker.ld
	clang $(LD_FLAGS) -o ../nightbeliever.krnl $(OBJ_FILES)

XboxKernel/KernelThunk.hpp: thunkgen.py XboxKernel/Kernel.hpp
	python thunkgen.py

clean:
	rm *.o XboxKernel/*.o || true
	rm ../nightbeliever.krnl || true
	rm XboxKernel/KernelThunk.cpp XboxKernel/KernelThunk.hpp || true
