# Every png file needs to have a matching obj file in the input folder
OUTPUTS := output/cubes.png
OUTPUTS += output/spheres.png
OUTPUTS += output/camaro.png
OUTPUTS += output/plant.png
OUTPUTS += output/audir8.png

WIDTH := 1920
HEIGHT := 1080
DEPTH := 3

OPTIMIZATION := 3

ifdef INPUT
ifndef OUTPUT
OUTPUT := output/$(patsubst %.obj,%.png,$(notdir $(INPUT)))
endif
endif

ifndef OUTPUT
OUTPUT := $(firstword $(OUTPUTS))
endif

ifndef INPUT
INPUT  := input/$(patsubst %.png,%.obj,$(notdir $(OUTPUT)))
endif

ARGUMENTS := -i $(INPUT) -o $(OUTPUT) -w $(WIDTH) -h $(HEIGHT) -d $(DEPTH)

SOURCE_DIR := src
BUILD_DIR  := cpurender
BINARY := $(BUILD_DIR)/cpurender
OPTIMIZATION := 3

CPUS := $(shell nproc)
MPIRUN := mpirun -np $(CPUS)

CXX := mpic++
FLAGS :=
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++11 -O$(OPTIMIZATION) $(FLAGS)

CXXFLAGS.valgrind := $(CXXFLAGS) -g
CXXFLAGS.gprof := $(CXXFLAGS) -g -pg -fno-omit-frame-pointer -fno-inline-functions -DNDEBUG

CHECK_FLAGS := $(BUILD_DIR)/.flags_$(shell echo '$(CXXFLAGS) $(DEFINES)' | md5sum | awk '{print $$1}')

SOURCE_PATHS := $(shell find $(SOURCE_DIR) -type f -name '*.cpp')
INCLUDE_DIRS := $(shell find $(SOURCE_DIR) -type f -name '*.hpp' -exec dirname {} \; | uniq)
OBJECTS     := $(SOURCE_PATHS:%.cpp=%.o)

INCLUDES := $(addprefix -I,$(INCLUDE_DIRS))

.PHONY: all verify call $(OUTPUTS) time gprof callgrind cachegrind .gprof .valgrind

help:
	@echo "TDT4200 Assignment 2"
	@echo ""
	@echo "Targets:"
	@echo "	all 		Builds $(BINARY)"
	@echo "	call		executes $(BINARY)"
	@echo "	clean		cleans up everything"
	@echo "	time		exeuction time"
	@echo "	gprof		gprof svg callgraph"
	@echo "	cachegrind	valgrind cachegrind using kcachegrind"
	@echo "	callgrind	valgrind callgrind using kcachegrind"
	@echo "	mpitest		executes mpitest"
	@echo ""
	@echo "Render Targets:"
	@echo "\t$(OUTPUTS)" | sed 's/ /\n\t/g'
	@echo ""
	@echo "Options:"
	@echo "	FLAGS=$(FLAGS)"
	@echo "	OPTIMIZATION=$(OPTIMIZATION)"
	@echo "	WIDTH=$(WIDTH)"
	@echo "	HEIGHT=$(HEIGHT)"
	@echo "	INPUT=$(INPUT)"
	@echo "	OUTPUT=$(OUTPUT)"
	@echo "	DEPTH=$(DEPTH)"
	@echo "	CPUS=$(CPUS)"
	@echo "	PROFILE=$(PROFILE)"
	@echo ""
	@echo "Compiler Call:"
	@echo "	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c dummy.cpp -o dummy.o"
	@echo "Binary Call:"
	@echo "	$(PROFILE) $(MPIRUN) $(BINARY) $(ARGUMENTS)"

all:
	@$(MAKE) --no-print-directory $(BINARY)


mpitest/mpitest: mpitest/main.cpp
	mpic++ $< -o $@

mpitest: mpitest/mpitest
	$(MPIRUN) $<

time: PROFILE := /usr/bin/time
time:
	@$(MAKE) --no-print-directory PROFILE="$(PROFILE)" call

gprof: PROFILE :=
gprof: CXXFLAGS := $(CXXFLAGS) -g -pg -fno-omit-frame-pointer -fno-inline-functions -DNDEBUG
gprof:
	@$(MAKE) --no-print-directory PROFILE="$(PROFILE)" CXXFLAGS="$(CXXFLAGS)" call
	gprof $(BINARY) gmon.out > $(BUILD_DIR)/gprof.txt
	@rm -f gmon.out
	utils/gprof2svg -s $(BUILD_DIR)/gprof.txt -o $(BUILD_DIR)/gprof.svg
	xdg-open $(BUILD_DIR)/gprof.svg 2>/dev/null


callgrind: PROFILE := valgrind --tool=callgrind --callgrind-out-file=$(BUILD_DIR)/callgrind.out
callgrind: CXXFLAGS := $(CXXFLAGS) -g
callgrind:
	@$(MAKE) --no-print-directory PROFILE="$(PROFILE)" CXXFLAGS="$(CXXFLAGS)" call
	kcachegrind $(BUILD_DIR)/callgrind.out >/dev/null 2>&1 &

cachegrind: PROFILE := valgrind --tool=cachegrind --cachegrind-out-file=$(BUILD_DIR)/cachegrind.out
cachegrind: CXXFLAGS := $(CXXFLAGS) -g
cachegrind: .valgrind
	@$(MAKE) --no-print-directory PROFILE="$(PROFILE)" CXXFLAGS="$(CXXFLAGS)" call
	kcachegrind $(BUILD_DIR)/cachegrind.out >/dev/null 2>&1 &

clean:
	rm -f $(BUILD_DIR)/.flags_*
	rm -f $(BINARY)
	rm -f $(OBJECTS)

.valgrind .gprof:
	@$(MAKE) --no-print-directory CXXFLAGS="${CXXFLAGS$@}" $(BINARY)

call: $(BINARY)
	$(PROFILE) $(MPIRUN) $(BINARY) $(ARGUMENTS)

$(OUTPUTS): $(BINARY)
	@$(MAKE) --no-print-directory INPUT=input/$(patsubst %.png,%.obj,$(notdir $@)) OUTPUT=$@  call

$(OBJECTS) : %.o : %.cpp $(CHECK_FLAGS)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(CHECK_FLAGS):
	@$(MAKE) --no-print-directory clean
	@mkdir -p $(dir $@)
	@touch $@

$(BINARY): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LINKING) -o $@

