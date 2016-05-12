CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)
CC_FLAGS := -std=c++14

all: NightBeliever zookeeper

zookeeper: $(OBJ_FILES)
	clang++ -g $(CC_FLAGS) -lboost_regex-mt -framework Hypervisor -o zookeeper $(OBJ_FILES)

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
