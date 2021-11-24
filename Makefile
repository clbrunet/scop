NAME = scop

CC = clang
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -I./src/ -I./ext/glad/include/ -I./ext/glfw-3.3.5/include/
CFLAGS += -Wno-unused-function -g3
CFLAGS += -fsanitize=address
DEPSFLAGS = -MMD -MP -MF $(@:.o=.d)

LDFLAGS = -lX11 -lpthread -ldl -lGL -lm -L./ext/glfw-3.3.5/build/src/ -lglfw3 -L./ext/glad/ -lglad
LIBGLFW3 = ext/glfw-3.3.5/build/src/libglfw3.a
LIBGLAD = ext/glad/libglad.a

SRCS = ./src/scop/main.c \
			 ./src/scop/utils.c \
			 ./src/scop/mat4.c \
			 ./src/scop/mat4x1.c \
			 ./src/scop/vec4.c \
			 ./src/scop/vec3.c \
			 ./src/scop/program.c \
			 ./src/scop/initialization.c \
			 ./src/scop/events.c \
			 ./src/scop/update.c \
			 ./src/scop/destruction.c

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all
all: $(NAME)

$(LIBGLFW3):
	cmake -S ./ext/glfw-3.3.5/ -B ./ext/glfw-3.3.5/build/
	make -C ./ext/glfw-3.3.5/build/

$(LIBGLAD):
	make -C ./ext/glad/

-include $(DEPS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -MF $(<:.c=.d) -o $@ -c $<

$(NAME): $(LIBGLFW3) $(LIBGLAD) $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
