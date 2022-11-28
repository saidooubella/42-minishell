
IFLAG=-I ~/.brew/Cellar/readline/8.2.1/include
LFLAG=-L ~/.brew/Cellar/readline/8.2.1/lib
FLAGS=-Wall -Wextra -Werror
NAME=minishell
CC=cc

SRCS=builtins.c cd_builtin_1.c cd_builtin_2.c echo_builtin.c element_1.c \
element_2.c env_builtin.c environment_1.c environment_2.c exec_resolver.c \
exit_builtin.c export_builtin.c ft_char_print_utils.c ft_number_print_utils.c \
ft_printf.c interpreter_1.c interpreter_2.c interpreter_3.c interpreter_4.c \
interpreter_5.c interpreter_6.c lexer1.c lexer2.c lexer_cases1.c lexer_cases2.c \
lexer_cases3.c lexer_cases4.c lexer_cases5.c lexer_cases6.c lexer_utils.c \
main_1.c main_2.c nodes_1.c nodes_2.c nodes_3.c parser_1.c parser_2.c parser_3.c \
parser_4.c parser_5.c parser_6.c parser_7.c path_simplifier_1.c path_simplifier_2.c \
pwd_builtin.c string.c string_builder1.c string_builder2.c string_utils_1.c \
string_utils_2.c string_utils_3.c string_utils_4.c string_utils_5.c tokens.c \
unset_builtin.c utils.c wildcard_matcher_1.c wildcard_matcher_2.c

HEADERS=builtins.h element.h environment.h exec_resolver.h ft_printf.h \
interpreter.h lexer.h lexer_cases.h lexer_utils.h main.h nodes.h parser.h \
path_simplifier.h string.h string_builder.h string_utils.h tokens.h utils.h \
wildcard_matcher.h

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@ $(CC) $(FLAGS) $(LFLAG) -lreadline $^ -o $@

%.o: %.c $(HEADERS)
	@ $(CC) $(FLAGS) $(IFLAG) -c $< -o $@

clean:
	@ rm -rf $(OBJS)

fclean: clean
	@ rm -rf $(NAME)

re: fclean all

check: all fclean

run: all clean
	@ ./minishell

.PHONY: all clean fclean re install-readline check run
