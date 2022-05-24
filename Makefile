CC     := gcc
CFLAGS := -v --std=c99 -ggdb

BUILD_DIR := build
WADER := $(BUILD_DIR)/wader

TEST_WAD := /home/bikkie/Documents/Mod/Quake/Wads/prototype_1_3.wad


.PHONY: all run debug


# TODO: make `.o` for each `.c` in `src/`
all: $(BUILD_DIR) $(WADER) $(BUILD_DIR)/lzss README

$(BUILD_DIR):
	mkdir -p $@

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
