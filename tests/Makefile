CC = gcc
BUILD_DIR ?= /tmp/freejoy-bcd-tests
CPPFLAGS = -I../application/Inc -include bcd_platform.h
CFLAGS = -std=c11 -g -O1 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer

.PHONY: test
test: $(BUILD_DIR)/test_bcd_buttons
	$(BUILD_DIR)/test_bcd_buttons

$(BUILD_DIR)/test_bcd_buttons: test_bcd_buttons.c bcd_platform.h ../application/Src/buttons.c $(wildcard ../application/Inc/*.h)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) test_bcd_buttons.c ../application/Src/buttons.c -o $@
