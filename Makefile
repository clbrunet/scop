NAME = scop

LIBGLFW3 = ext/glfw-3.3.5/build/src/libglfw3.a
LIBGLAD = ext/glad/libglad.a

CC = clang
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -I./src/ -I./ext/glad/include/ -I./ext/glfw-3.3.5/include/
CFLAGS += -Wno-unused-function -g3
CFLAGS += -fsanitize=address
DEPSFLAGS = -MMD -MP -MF $(@:.o=.d)
LDFLAGS = -L./ext/glfw-3.3.5/build/src/ -L./ext/glad/
LDLIBS = -lX11 -lpthread -ldl -lGL -lm -lglfw3 -lglad

SRCS = ./src/scop/main.c \
			 ./src/scop/utils.c \
			 ./src/scop/matrices/mat4.c \
			 ./src/scop/matrices/mat4x1.c \
			 ./src/scop/vectors/vec3.c \
			 ./src/scop/vectors/vec4.c \
			 ./src/scop/program.c \
			 ./src/scop/tga.c \
			 ./src/scop/load_model.c \
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
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(NAME) $(OBJS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all
