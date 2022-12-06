
IFLAG=-I ~/.brew/Cellar/readline/8.2.1/include -I includes
LFLAG=-L ~/.brew/Cellar/readline/8.2.1/lib -lreadline
FLAGS=-Wall -Wextra -Werror -MMD
BONUS_NAME=minishell_bonus
NAME=minishell
CC=cc

COMMON=entry_point/entry_point_1.c entry_point/entry_point_2.c \
lexer/lexer_cases6.c lexer/lexer_utils.c lexer/tokens.c parser/nodes/nodes_1.c \
utils/exec_resolver/exec_resolver.c utils/path_simplifier/path_simplifier_1.c \
utils/path_simplifier/path_simplifier_2.c utils/printf/ft_char_print_utils.c \
parser/parser_2.c parser/parser_3.c parser/parser_4.c parser/parser_5.c \
lexer/lexer1.c lexer/lexer2.c lexer/lexer_cases1.c lexer/lexer_cases2.c \
utils/strings/free_aware_string.c utils/strings/string_builder1.c \
interpreter/interpreter_1.c utils/wildcards/wildcard_matcher_2.c \
parser/nodes/nodes_2.c parser/nodes/nodes_3.c parser/parser_1.c \
lexer/lexer_cases3.c lexer/lexer_cases4.c lexer/lexer_cases5.c \
utils/strings/string_builder2.c utils/strings/string_utils_1.c \
parser/parser_6.c parser/parser_7.c utils/builtins/builtins.c \
utils/builtins/exit_builtin.c utils/builtins/export_builtin.c \
utils/printf/ft_number_print_utils.c utils/printf/ft_printf.c \
utils/strings/string_utils_2.c utils/strings/string_utils_3.c \
utils/strings/string_utils_4.c utils/strings/string_utils_5.c \
utils/builtins/pwd_builtin.c utils/builtins/unset_builtin.c \
utils/builtins/cd_builtin_1.c utils/builtins/cd_builtin_2.c \
utils/builtins/echo_builtin.c utils/builtins/env_builtin.c \
interpreter/interpreter_2.c interpreter/interpreter_3.c \
interpreter/interpreter_4.c interpreter/interpreter_5.c \
interpreter/interpreter_6.c interpreter/interpreter_7.c \
utils/elements/element_1.c utils/elements/element_2.c \
utils/env/environment_1.c utils/env/environment_2.c \
utils/utils.c utils/wildcards/wildcard_matcher_1.c

ALL_FILES=$(COMMON) minishell_bonus.c minishell.c

DEPS_FILES=$(ALL_FILES:.c=.d)

OBJS_FILES=$(ALL_FILES:.c=.o)

BONUS_OBJS=$(COMMON:.c=.o) minishell_bonus.o

MAN_OBJS=$(COMMON:.c=.o) minishell.o

all: $(NAME)

$(NAME): $(MAN_OBJS)
	$(CC) $(LFLAG) $^ -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CC) $(LFLAG) $^ -o $@

%.o: %.c
	$(CC) $(FLAGS) $(IFLAG) -c $< -o $@

clean:
	rm -f $(OBJS_FILES) $(DEPS_FILES)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)

re: fclean all

check: all fclean

run: all clean
	./minishell

-include $(DEPS_FILES)

.PHONY: all bonus clean fclean re check run
