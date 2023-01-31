CC := gcc -std=gnu99 -I./inc
CFLAGS := -Werror -Wall -pedantic

OUT_DIR := out
SRC_DIR := src

OBJS := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
OBJS := $(filter-out $(OUT_DIR)/main.o,$(OBJS))
DEPENDENCIES := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.d,$(wildcard $(SRC_DIR)/*.c))

UNIT_TESTS_DIR := unit_tests
UNIT_TEST_OBJS := $(patsubst $(UNIT_TESTS_DIR)/%.c,$(OUT_DIR)/%.o,$(wildcard $(UNIT_TESTS_DIR)/*.c))
UNIT_TESTS_DEPENDENCIES := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.d,$(wildcard $(SRC_DIR)/*.c))
UNIT_TESTS_LIBS := -lcheck -lpthread -lrt

.PHONY: clean debug release

app.exe: $(OUT_DIR) $(OBJS) $(DEPENDENCIES) $(OUT_DIR)/main.o
	$(CC) $(CFLAGS) $(OBJS) $(OUT_DIR)/main.o -o $@

unit_tests.exe: CFLAGS += -O0 -g -DDEBUG
unit_tests.exe: $(OUT_DIR) $(OBJS) $(UNIT_TEST_OBJS) $(UNIT_TESTS_DEPENDENCIES)
	$(CC) $(CFLAGS) $(OBJS) $(UNIT_TEST_OBJS) $(UNIT_TESTS_LIBS) -o $@

unit: UNIT_TESTS_LIBS += -lsubunit -lm
unit: unit_tests.exe
	./unit_tests.exe

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.o: $(UNIT_TESTS_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.d: $(SRC_DIR)/%.c | $(OUT_DIR)
	$(CC) -MM $< -MF"$@" -MT"$(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$<)"

$(OUT_DIR)/%.d: $(UNIT_TESTS_DIR)/%.c | $(OUT_DIR)
	$(CC) -MM $< -MF"$@" -MT"$(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$<)"

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

ifeq (,$(filter clean,$(MAKECMDGOALS)))
include $(DEPENDENCIES)
endif

debug: CFLAGS += -O0 -g -DDEBUG
debug: app.exe

release: app.exe

clean:
	rm -rf ./*.exe $(OUT_DIR)
