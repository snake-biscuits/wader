CC     := gcc
CFLAGS := --std=c99 -ggdb

BUILD_DIR := build

WADER := $(BUILD_DIR)/wader

TEST_WAD := /home/bikkie/Documents/Mod/Quake/Wads/prototype_1_3.wad

DUMMY := mkdir -p $(BUILD_DIR)

.PHONY: all run debug


# TODO: make `.o` for each `.c` in `src/`
all: $(BUILD_DIR)/* README

$(WADER): src/main.c src/wadfile.h src/common.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/lzss: src/lzss.c
	$(CC) $(CFLAGS) -DLZSS_MAIN $< -o $@

README: $(WADER)
	$(WADER) -v > $@; echo >> $@; $(WADER) -h >> $@

run: $(WADER)
	$< -l $(TEST_WAD) | less -

debug: $(WADER)
	gdb -ex bt -ex run --args $(WADER) -l $(TEST_WAD)

# TODO: groff manpage
# TODO: installer
# TODO: test scripts (see mdolidon/endlines for examples)
