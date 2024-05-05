# ↓ Basic variables
CC ?= gcc
CFLAGS := -std=gnu11
CFLAGS += -W -Wall -Wextra -Wpedantic
CFLAGS += -Wundef -Wshadow -Wcast-align -Wunused
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Waggregate-return -Wcast-qual
CFLAGS += -Wunreachable-code
CFLAGS += -U_FORTIFY_SOURCE
CFLAGS += -iquote ./src

LDLIBS := -lraylib
LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl
LDFLAGS :=

# ↓ Binaries
NAME ?= gameoflife
ASAN_NAME := asan
PROF_NAME := prof

# Source files
SRC := $(shell find ./src -name '*.c')

# ↓ Objects
BUILD_DIR := .build
OBJ := $(SRC:%.c=$(BUILD_DIR)/source/%.o)
ASAN_OBJ := $(SRC:%.c=$(BUILD_DIR)/asan/%.o)
PROF_OBJ := $(SRC:%.c=$(BUILD_DIR)/prof/%.o)

# ↓ Dependencies for headers
DEPS_FLAGS := -MMD -MP
DEPS := $(OBJ:.o=.d)
ASAN_DEPS := $(ASAN_OBJ:.o=.d)
PROF_DEPS := $(PROF_OBJ:.o=.d)

.DEFAULT_GOAL := all
.PHONY: all
all: $(NAME)

# ↓ Compiling
$(BUILD_DIR)/source/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(LDLIBS) $(DEPS_FLAGS)

$(NAME): CFLAGS += -flto
$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)

# ↓ Asan
$(BUILD_DIR)/asan/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(LDLIBS) $(DEPS_FLAGS)

$(ASAN_NAME): CFLAGS += -fsanitize=address,leak,undefined -g3
$(ASAN_NAME): CFLAGS += -fanalyzer
$(ASAN_NAME): CFLAGS += -D DEBUG_MODE
$(ASAN_NAME): $(ASAN_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)

# ↓ Profiler
$(BUILD_DIR)/prof/%.o: %.c
	@ mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(LDLIBS) $(DEPS_FLAGS)

$(PROF_NAME): CFLAGS += -pg
$(PROF_NAME): $(PROF_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)

# ↓ Coverage
.PHONY: cov
cov: GCOVR_FLAGS := --exclude bonus/
cov: GCOVR_FLAGS += --exclude tests/
cov:
	@ gcovr $(GCOVR_FLAGS)
	@ gcovr $(GCOVR_FLAGS) --branches

# ↓ Cleaning
.PHONY: clean
clean:
	@ $(RM) -r $(BUILD_DIR)

.PHONY: fclean
fclean: clean
	@ $(RM) $(NAME) $(ASAN_NAME) $(PROF_NAME)

.PHONY: re
.NOTPARALLEL: re
re: fclean all

-include $(DEPS)
-include $(ASAN_DEPS)
-include $(PROF_DEPS)
