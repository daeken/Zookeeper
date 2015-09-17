CPP_FILES := $(wildcard *.cpp)
ASM_FILES := $(wildcard *.S)
OBJ_FILES := $(CPP_FILES:.cpp=.o) $(ASM_FILES:.S=.o)
CC_FLAGS := -std=c++11 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I/usr/local/gcc-4.8.1-for-linux32/include -I/usr/local/gcc-4.8.1-for-linux32/include/c++/4.8.1/ -I/usr/local/gcc-4.8.1-for-linux32/include/c++/4.8.1/i586-pc-linux/ -Iinclude
LD_FLAGS := -T linker.ld -ffreestanding -fno-use-linker-plugin -O2 -nostdlib
PATH := /usr/local/gcc-4.8.1-for-linux32/libexec/gcc/i586-pc-linux/4.8.1:/usr/local/gcc-4.8.1-for-linux32/i586-pc-linux/bin:$(PATH)

%.o: %.cpp
	@PATH=$(PATH); \
	g++ $(CC_FLAGS) -c -o $@ $<

%.o: %.S
	@PATH=$(PATH); \
	as -o $@ $<

all: $(OBJ_FILES) linker.ld
	@PATH=$(PATH); \
	g++ $(LD_FLAGS) -o ../nightbeliever.krnl $^

clean:
	rm *.o
