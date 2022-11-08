
CC=cc
FLAGS=-Wall -Wextra -Werror
NAME=minishell

SRCS=lexer_cases5.c lexer_cases6.c lexer_utils.c main.c string_builder1.c string_builder2.c \
tokens.c utils.c ft_char_print_utils.c ft_number_print_utils.c ft_printf.c lexer1.c lexer2.c \
lexer_cases1.c lexer_cases2.c lexer_cases3.c lexer_cases4.c string_utils.c interpreter.c parser.c

HEADERS=lexer.h lexer_cases.h lexer_utils.h string_builder.h string_utils.h tokens.h utils.h \
ft_printf.h interpreter.h parser.h

OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -lreadline -fsanitize=address $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

install-readline:
	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
	brew update --force
	brew install readline

check: all fclean

run:
	make all
	make clean
	./minishell

.PHONY: all clean fclean re install-readline
