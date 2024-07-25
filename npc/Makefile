all:
	@echo "Write this Makefile by your self."

sim:
	@verilator --cc --exe --build -j 4 -Wall csrc/main.cpp vsrc/top.v --trace
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
include ../Makefile
