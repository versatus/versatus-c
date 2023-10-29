CC = gcc
CC_WASM = emcc

CFLAGS = -std=c99 
JSON_C_DIR = /usr/local
CFLAGS += -I$(JSON_C_DIR)/include

WASM_SRC = contract.wasm
WASM2WAT = wasm2wat

LDFLAGS += -L$(JSON_C_DIR)/lib -ljson-c

SRC = main.c
HDRS = ../src/versatus_c.h

all: local contract.wasm contract.wat

local: $(SRC) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
contract.wasm: $(SRC) $(HDRS)
	$(CC_WASM) $(CFLAGS) -o $@ $< $(LDFLAGS)

contract.wat: $(WASM_SRC)
	$(WASM2WAT) $<  -o $@

clean:
	rm -f local contract.wasm contract.wat

.PHONY: clean

