CSRC = $(shell find ./csrc -name "*.cpp" -or -name "*.c" -or -name "*.cc")
VSRC = $(shell find ./vsrc -name "*.v" -or -name ".sv")
BUILD_DIR = ./build
LOG = -l $(NPC_HOME)/log/npc_log.txt
VFLAGS = -cc -trace --exe --build -j 4 --top-module top #-Wall 
all:
	@echo "Write this Makefile by your self."
build:
	@echo verilator $(VFLAGS) $(CSRC) $(VSRC) \
		-Mdir $(BUILD_DIR) \
		-CFLAGS -I$(NPC_HOME)/csrc/include
	@verilator $(VFLAGS) $(CSRC) $(VSRC) \
		-Mdir $(BUILD_DIR) \
		-CFLAGS -I$(NPC_HOME)/csrc/include \
		-LDFLAGS -"lreadline" \
		-LDFLAGS -"lLLVM-14"
run: clean build
	$(BUILD_DIR)/Vtop $(LOG) 
sim: run
	gtkwave wave.vcd
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
clean: 
	@rm -rf $(BUILD_DIR) 
	@rm -f *.vcd
   # @rm -rf ./log
include ../Makefile
