
IFLAG=-I ~/.brew/Cellar/readline/8.2.1/include
LFLAG=-L ~/.brew/Cellar/readline/8.2.1/lib
FLAGS=-Wall -Wextra -Werror# -fsanitize=address
NAME=minishell
CC=cc

SRCS=builtins.c environment.c exec_resolver.c ft_char_print_utils.c ft_number_print_utils.c \
ft_printf.c interpreter.c lexer1.c lexer2.c lexer_cases1.c lexer_cases2.c lexer_cases3.c \
lexer_cases4.c lexer_cases5.c lexer_cases6.c lexer_utils.c main.c nodes.c parser.c string.c \
string_builder1.c string_builder2.c string_utils.c tokens.c utils.c wildcard_matcher.c \
element.c path_simplifier.c

HEADERS=builtins.h environment.h exec_resolver.h ft_printf.h interpreter.h lexer.h \
lexer_cases.h lexer_utils.h main.h nodes.h parser.h string.h string_builder.h string_utils.h \
tokens.h utils.h wildcard_matcher.h element.h path_simplifier.h

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

# install-readline:
# 	~/Cleaner_42.sh
# 	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
# 	brew update --force
# 	brew install readline

check: all fclean

run: all clean
	@ ./minishell

.PHONY: all clean fclean re install-readline check run
