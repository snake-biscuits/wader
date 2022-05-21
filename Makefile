CC     := gcc
CFLAGS := --std=c99 -ggdb

BUILD_DIR := build

WADER := $(BUILD_DIR)/wader

TEST_WAD := /home/bikkie/Documents/Mod/Quake/Wads/prototype_1_3.wad

DUMMY := mkdir -p $(BUILD_DIR)

.PHONY: run debug


$(WADER): src/main.c src/wadfile.h src/common.h
	$(CC) $(CFLAGS) $< -o $@

run: $(WADER)
	$< $(TEST_WAD)

debug:
	gdb -ex run --args $(WADER) $(TEST_WAD)

# TODO: groff manpage
# TODO: installer
