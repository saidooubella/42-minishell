
IFLAG=-I ~/.brew/Cellar/readline/8.2.1/include
LFLAG=-L ~/.brew/Cellar/readline/8.2.1/lib
FLAGS=-Wall -Wextra -Werror -g
NAME=minishell
CC=cc

SRCS=lexer_cases5.c lexer_cases6.c lexer_utils.c main.c string_builder1.c string_builder2.c \
tokens.c utils.c ft_char_print_utils.c ft_number_print_utils.c ft_printf.c lexer1.c lexer2.c \
lexer_cases1.c lexer_cases2.c lexer_cases3.c lexer_cases4.c string_utils.c interpreter.c parser.c \
string.c nodes.c builtins.c

HEADERS=lexer.h lexer_cases.h lexer_utils.h string_builder.h string_utils.h tokens.h utils.h \
ft_printf.h interpreter.h parser.h string.h nodes.h builtins.h

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(LFLAG) -lreadline $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) $(IFLAG) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

# install-readline:
# 	~/Cleaner_42.sh
# 	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
# 	brew update --force
# 	brew install readline

check: all fclean

run: all clean
	./minishell

.PHONY: all clean fclean re install-readline check run
