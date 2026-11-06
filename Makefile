NAME			= codexion
CC				= cc
CFLAGS			= -Wall -Wextra -Werror -pthread
INCLUDE			= -I includes
OBJ_DIR			= obj
C_FILES			= srcs/codexion.c \
				  srcs/parser.c \
				  srcs/time.c \
				  srcs/coders.c \
				  srcs/coders_burnout.c \
				  srcs/dongles.c \
				  srcs/simulation.c \
				  srcs/inits.c \
				  srcs/monitor.c \
				  srcs/heap.c \
				  srcs/heap_insert.c \
				  srcs/utils.c

O_FILES			= $(patsubst srcs/%.c,$(OBJ_DIR)/srcs/%.o,$(C_FILES))
D_FILES			= $(O_FILES:.o=.d)

all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/srcs/%.o: srcs/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -MMD -MP -c $< -o $@

$(NAME): $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

-include $(D_FILES)

.PHONY: all re clean fclean