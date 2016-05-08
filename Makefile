CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)
CC_FLAGS := -std=c++11

all: NightBeliever $(OBJ_FILES)
	clang++ -g $(CC_FLAGS) -framework Hypervisor -o zookeeper $(OBJ_FILES)

.PHONY: NightBeliever

NightBeliever:
	$(MAKE) -C NightBeliever

%.o: %.cpp
	clang++ $(CC_FLAGS) -c -g -o $@ $<

clean:
	$(MAKE) -C NightBeliever clean
	rm zookeeper
	rm *.o
