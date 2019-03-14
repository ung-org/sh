%{
#include "sh.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Thanks, Bison, for implicitly declaring identifiers */
extern int yylex(void);
extern int yyerror(const char *);
extern char *yytext;
%}

%union {
	int n;
	char *s;
	struct sh_command *cmd;
	struct sh_redirect *redir;
}

%token<s> WORD ASSIGNMENT_WORD NAME
%token NEWLINE
%token<n> IO_NUMBER
%token AND_IF OR_IF DSEMI
%token DLESS DGREAT LESSAND GREATAND LESSGREAT DLESSDASH
%token CLOBBER
%token If Then Else Elif Fi Do Done
%token Case Esac While Until For
%token Lbrace Rbrace Bang
%token In

 /* %type<cmd> simple_command */
%type<s> filename here_end cmd_name cmd_word
%type<redir> io_redirect io_file io_here

%start complete_command
%%
complete_command : list separator
	| list
	;

list	: list separator_op and_or
	| and_or
	;

and_or	: pipeline
	| and_or AND_IF linebreak pipeline
	| and_or OR_IF linebreak pipeline
	;

pipeline : pipe_sequence
	| Bang pipe_sequence
	;

pipe_sequence : command
	| pipe_sequence '|' linebreak command
	;

command	: simple_command 
	| compound_command
	| compound_command redirect_list
	| function_definition
	;

compound_command : brace_group
	| subshell
	| for_clause
	| case_clause
	| if_clause
	| while_clause
	| until_clause
	;

subshell : '(' compound_list ')'
	;

compound_list : term
	| newline_list term
	| term separator
	| newline_list term separator
	;

term	: term separator and_or
	| and_or
	;

for_clause : For name linebreak do_group
	| For name linebreak in sequential_sep do_group
	| For name linebreak in wordlist sequential_sep do_group
	;

name	: NAME { /* apply rule 5 */
		printf("NAME: %s\n", yytext);
	}
	;

in	: In	/* apply rule 6 */
	;

wordlist : wordlist WORD
	| WORD {
		printf("WORD: %s\n", yytext);
	}
	;

case_clause : Case WORD linebreak in linebreak case_list Esac
	| Case WORD linebreak in linebreak case_list_ns Esac
	| Case WORD linebreak in linebreak Esac
	;

case_list_ns : case_list case_item_ns
	| case_item_ns
	;

case_list : case_list case_item
	| case_item
	;

case_item_ns : pattern ')' linebreak
	| pattern ')' compound_list linebreak
	| '(' pattern ')' linebreak
	| '(' pattern ')' compound_list linebreak
	;

case_item : pattern ')' linebreak DSEMI linebreak
	| pattern ')' compound_list DSEMI linebreak
	| '(' pattern ')' linebreak DSEMI linebreak
	| '(' pattern ')' compound_list DSEMI linebreak
	;

pattern	: WORD	/* apply rule 4 */
	| pattern '|' WORD	/* do *NOT* apply rule 4 */
	;

if_clause : If compound_list Then compound_list else_part Fi
	| If compound_list Then compound_list Fi
	;

else_part : Elif compound_list Then compound_list
	| Elif compound_list Then compound_list else_part
	| Else compound_list
	;

while_clause : While compound_list do_group
	;

until_clause : Until compound_list do_group
	;

function_definition : fname '(' ')' linebreak function_body

function_body : compound_command	/* apply rule 9 */
	| compound_command redirect_list /* apply rule 9 */
	;

fname	: NAME				/* apply rule 8 */
	;

brace_group : Lbrace compound_list Rbrace
	;

do_group : Do compound_list Done	/* apply rule 6 */
	;

simple_command : cmd_prefix cmd_word cmd_suffix
	| cmd_prefix cmd_word {
		printf("simple_command:cmd_prefix '%s'\n", $2);
	}
	| cmd_prefix
	| cmd_name cmd_suffix {
		printf("simple_command:'%s' cmd_suffix\n", $1);
	}
	| cmd_name {
		printf("simple_command:'%s'\n", $1);
	}
	;

cmd_name : WORD {
		/* rule 7a */
		$$ = strdup($1);
	}
	;

cmd_word : WORD {
		/* rule 7b */
		$$ = strdup($1);
	}
	;

cmd_prefix : io_redirect
	| cmd_prefix io_redirect
	| ASSIGNMENT_WORD
	| cmd_prefix ASSIGNMENT_WORD
	;

cmd_suffix : io_redirect
	| cmd_suffix io_redirect
	| WORD {
		printf("cmd_suffix:WORD:%s\n", $1);
	}
	| cmd_suffix WORD {
		printf("cmd_suffix:cmd_suffix WORD:%s\n", $2);
	}
	;

redirect_list : io_redirect {
		printf("Redirect fd %d, dir %d, name %s\n", $1->fd, $1->direction, $1->name);
	}
	| redirect_list io_redirect {
		printf("Redirect fd %d, dir %d, name %s\n", $2->fd, $2->direction, $2->name);
	}
	;

io_redirect : io_file {
		$$ = $1;
		printf("Redirect fd %d to %s\n", $1->fd, $1->name);
	}
	| IO_NUMBER io_file {
		$2->fd = $1;
		printf("Redirect fd %d to %s\n", $2->fd, $2->name);
		$$ = $2;
	}
	| io_here {
		printf("Redirect fd %d until %s\n", $1->fd, $1->name);
		$$ = $1;
	}
	| IO_NUMBER io_here {
		$2->fd = $1;
		printf("Redirect fd %d until %s\n", $2->fd, $2->name);
		$$ = $2;
	}
	;

io_file : '<' filename {
		$$ = sh_add_redirect(STDIN_FILENO, READ, $2);
	}
	| LESSAND filename {
		$$ = sh_add_redirect(STDIN_FILENO, READFD, $2);
	}
	| '>' filename {
		$$ = sh_add_redirect(STDOUT_FILENO, WRITE, $2);
	}
	| GREATAND filename {
		$$ = sh_add_redirect(STDOUT_FILENO, WRITEFD, $2);
	}
	| DGREAT filename {
		$$ = sh_add_redirect(STDOUT_FILENO, APPEND, $2);
	}
	| LESSGREAT filename {
		$$ = sh_add_redirect(-1, READWRITE, $2);
	}
	| CLOBBER filename {
		$$ = sh_add_redirect(STDOUT_FILENO, OVERWRITE, $2);
	}
	;

filename : WORD {
		/* rule 2 */
		$$ = yytext;
	}
	;

io_here	: DLESS here_end {
		$$ = sh_add_redirect(STDIN_FILENO, READHERE, $2);
	}
	| DLESSDASH here_end {
		$$ = sh_add_redirect(STDIN_FILENO, READDASH, $2);
	}
	;

here_end : WORD {
		/* rule 3 */
		$$ = yytext;
	}
	;

newline_list : NEWLINE {
		puts("newline_list:NEWLINE");
	}
	| newline_list NEWLINE {
		puts("newline_list:newline_list:NEWLINE");
	}
	;

linebreak : newline_list {
		puts("linebreak:newline_list");
	}
	| /* empty */
	;

separator_op : '&' {
		puts("separator_op:&");
	}
	| ';' {
		puts("separator_op:;");
	}
	;

separator : separator_op linebreak {
		puts("separator: separator_op linebreak");
	}
	| newline_list {
		puts("separator:newline_list");
	}
	;

sequential_sep : ';' linebreak {
		puts("sequential_sep:; linebreak");
	}
	| newline_list {
		puts("sequential_sep:newline_list");
	}
	;
