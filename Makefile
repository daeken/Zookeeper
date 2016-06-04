CPP_FILES := $(wildcard *.cpp)
CC_FLAGS := -std=c++14 -I.

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CC_FLAGS += -DLINUX
	CPP_FILES += $(wildcard Linux/*.cpp)
endif
ifeq ($(UNAME_S),Darwin)
	CC_FLAGS += -DMAC
	CPP_FILES += $(wildcard Mac/*.cpp)
endif

OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: NightBeliever zookeeper_libretro.dylib

zookeeper_libretro.dylib: $(OBJ_FILES)
	clang++ -g $(CC_FLAGS) -dynamiclib -lboost_regex-mt -framework Hypervisor -o zookeeper_libretro.dylib $(OBJ_FILES)

.PHONY: NightBeliever

NightBeliever: Hypercall.hpp
	$(MAKE) -C NightBeliever

%.o: %.cpp
	clang++ $(CC_FLAGS) -c -g -o $@ $<

Hypercall.hpp: hypercallgen.py hypercalls.yaml
	python hypercallgen.py

clean:
	$(MAKE) -C NightBeliever clean
	rm *.o || true
	rm zookeeper || true
