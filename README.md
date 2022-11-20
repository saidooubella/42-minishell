# - Minishell

This project aims to create a tiny version of the well-known shell `bash`. With this, the student
will get into the world of parsers and interpreters.

# - Walkthrough

To walk through this project start from **`main.c`** and make your way to **`lexer2.c`**... **`parser.c`**...
and finally **`interpreter.c`**. Other files were used as helpers, you will find yourself jumping
back and forth between them while walking through the source files.

# - Building & Running

Use `make` to build and produce an executable or `make run` to build and execute immediately.
Use `make fclean` to remove any files produced by building the program.

# - The Grammar:

```
expresion	:	and_or
			;

and_or		:	pipeline ('&&' | '||') pipeline
			;

pipeline	:	primary '|' primary
			;

primary		:	WORD
			|	'"([^"]|$[a-zA-Z_][a-zA-Z0-9_]*)*"'
			|	"'([^'])*'"
			|	'(' expresion ')'
			|	'$'  [a-zA-Z_][a-zA-Z0-9_]*
			|	'<<' LIMITER
			|	'>>' FILE
			|	'>'	 FILE
			|	'<'	 FILE
			;
```
