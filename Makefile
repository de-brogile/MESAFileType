CC = gcc
CFLAGS = -Wall -Wextra -I.
LDFLAGS = -lmagic -lpthread -lcurl

# 源文件
SRCS = \
    src/MESA_file_type.c \
    src/frontend_identification.c \
    src/backend_identification.c \
    src/identification_core.c \
    utils/progress_bar.c \
    test/single_file_test.c \
    test/single_url_test.c \
    test/directory_test.c \
    test/urls_file_test.c \
    test/main_test.c

# 目标文件
OBJS = $(SRCS:.c=.o)

# 可执行文件
EXEC = file_type_test

.PHONY: all clean rebuild test

all: $(EXEC)

# 编译每个 .c 文件为 .o 文件
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 链接所有目标文件生成可执行文件
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS) $(EXEC)

rebuild: clean all

test: $(EXEC)
	./$(EXEC)