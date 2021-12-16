NAME := scop
SRC_DIR := ./src
BUILD_DIR := ./build

LIBGLFW3 := ext/glfw-3.3.5/build/src/libglfw3.a
LIBGLAD := ext/glad/libglad.a

CC := clang
CFLAGS := -Wall -Wextra -Werror
CFLAGS += -I$(SRC_DIR)/ -I./ext/glad/include/ -I./ext/glfw-3.3.5/include/
CFLAGS += -Wno-unused-parameter -Wno-unused-function -g3
# CFLAGS += -fsanitize=address
LDFLAGS := -L./ext/glfw-3.3.5/build/src/ -L./ext/glad/
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDLIBS := -lX11 -lpthread -ldl -lGL -lm -lglfw3 -lglad
endif
ifeq ($(UNAME_S),Darwin)
	LDLIBS := -framework Cocoa -framework OpenGL -framework IOKit -lglfw3 -lglad
endif

SRCS := $(shell find $(SRC_DIR)/scop/ -type f -name "*.c")
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all
all: $(NAME)

$(LIBGLFW3):
	cmake -S ./ext/glfw-3.3.5/ -B ./ext/glfw-3.3.5/build/
	make -C ./ext/glfw-3.3.5/build/

$(LIBGLAD):
	make -C ./ext/glad/

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -MF $(@:.o=.d) -o $@ -c $<

$(NAME): $(LIBGLFW3) $(LIBGLAD) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(NAME) $(OBJS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
