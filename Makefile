CC     := gcc
CFLAGS := --std=c99 -ggdb

BUILD_DIR := build
WADER := $(BUILD_DIR)/wader

TEST_WAD := /home/bikkie/Documents/Mod/Quake/Wads/prototype_1_3.wad


.PHONY: all run debug


# TODO: make `.o` for each `.c` in `src/`
all: $(BUILD_DIR) $(WADER) README $(BUILD_DIR)/lzss $(BUILD_DIR)/vpk

$(BUILD_DIR):
	mkdir -p $@

$(WADER): src/main.c src/wadfile.h src/common.h src/version.h
	$(CC) $(CFLAGS) $< -o $@

README: $(WADER)
	$(WADER) -V > $@; echo >> $@; $(WADER) -h >> $@

$(BUILD_DIR)/lzss: src/lzss.c
	$(CC) $(CFLAGS) -DLZSS_MAIN $< -o $@

$(BUILD_DIR)/vpk: src/vpkfile.c
	$(CC) $(CFLAGS) -DVPK_MAIN $< -o $@

run: $(WADER)
	$< -l $(TEST_WAD) | less -

debug: $(WADER)
	gdb -ex bt -ex run --args $(WADER) -l $(TEST_WAD)

# TODO: groff manpage
# TODO: installer
# TODO: test scripts (see mdolidon/endlines for examples)
