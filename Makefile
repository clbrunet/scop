TARGET = scop

CC = clang
CFLAGS = -Wall -Wextra -Werror -I./src/ -I./ext/glad/include/ -Wno-unused-function -g3
LDFLAGS = -lGL -lglfw -ldl
DEPSFLAGS = -MMD -MP -MF $(@:.o=.d)

SRCS = ext/glad/src/glad.c \
			 src/scop/main.c

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

.PHONY: all
all: $(TARGET)

-include $(DEPS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPSFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS)

.PHONY: fclean
fclean: clean
	rm -f $(TARGET)

.PHONY: re
re: fclean all
