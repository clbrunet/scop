NAME = scop

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./src/ -I./ext/glad/include/ -I./ext/glfw/include/
CFLAGS += -Wno-unused-function -g3
DEPSFLAGS = -MMD -MP -MF $(@:.o=.d)

LDFLAGS = -lpthread -ldl -lGL -lm -L./ext/glfw/build/src/ -lglfw3 -L./ext/glad/ -lglad
LIBGLFW3 = ext/glfw/build/src/libglfw3.a
LIBGLAD = ext/glad/libglad.a

SRCS = src/scop/main.c

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all
all: $(NAME)

$(LIBGLFW3):
	cmake -S ./ext/glfw -B ./ext/glfw/build
	make -C ./ext/glfw/build/

$(LIBGLAD):
	make -C ./ext/glad/

-include $(DEPS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -MF $(<:.c=.d) -o $@ -c $<

$(NAME): $(OBJS) $(LIBGLFW3) $(LIBGLAD)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
