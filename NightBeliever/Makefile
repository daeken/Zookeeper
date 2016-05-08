CPP_FILES := $(wildcard *.cpp)
ASM_FILES := $(wildcard *.S)
OBJ_FILES := $(CPP_FILES:.cpp=.o) $(ASM_FILES:.S=.o)
CC_FLAGS := -std=c++11 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I/usr/local/gcc-4.8.1-for-linux32/include -I/usr/local/gcc-4.8.1-for-linux32/include/c++/4.8.1/ -I/usr/local/gcc-4.8.1-for-linux32/include/c++/4.8.1/i586-pc-linux/ -Iinclude
LD_FLAGS := -T linker.ld -ffreestanding -fno-use-linker-plugin -O2 -nostdlib

%.o: %.cpp
	i686-elf-g++ $(CC_FLAGS) -c -o $@ $<

%.o: %.S
	i686-elf-as -o $@ $<

all: $(OBJ_FILES) linker.ld
	i686-elf-g++ $(LD_FLAGS) -o ../nightbeliever.krnl $^

clean:
	rm *.o
