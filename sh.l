%{
#include "sh.h"
#include "sh.tab.h"
/* thanks, flex, for implicitly declaring identifiers */
/* bonus points for something that isn't part of ISO C */
extern int fileno(FILE *);
%}

NAME [_a-zA-Z0-9]
NONDIGIT [_a-zA-Z]
DIGIT [0-9]
WHITESPACE [ \t]
OPERATOR [()|;&<>]

%s FOR
%x COMMENT

%%

"#"		{ BEGIN COMMENT; }
<COMMENT>.	;
<COMMENT>\n	{ BEGIN INITIAL; }

"&&"/.	{ return AND_IF; }
"||"/.	{ return OR_IF; }
";;"/.	{ return DSEMI; }
"<<"/.	{ return DLESS; }
">>"/.	{ return DGREAT; }
"<&"/.	{ return LESSAND; }
">&"/.	{ return GREATAND; }
"<>"/.	{ return LESSGREAT; }
"<<-"/.	{ return DLESSDASH; }
">|"/.	{ return CLOBBER; }

"if"	{ return If; }
"then"	{ return Then; }
"else"	{ return Else; }
"elif"	{ return Elif; }
"fi"	{ return Fi; }
"do"	{ return Do; }
"done"	{ BEGIN INITIAL; return Done; }
"case"	{ return Case; }
"esac"	{ return Esac; }
"while"	{ return While; }
"until"	{ return Until; }
"for"	{ BEGIN FOR; return For; }

"{"	{ return Lbrace; }
"}"	{ return Rbrace; }
"!"	{ return Bang; }

"in"	{ return In; }

{OPERATOR}/.	{ return yytext[0]; }

{DIGIT}+/<	{ yylval.n = atoi(yytext); return IO_NUMBER; }
{DIGIT}+/>	{ yylval.n = atoi(yytext); return IO_NUMBER; }

\n		{ return NEWLINE; }

<FOR>{NONDIGIT}{NAME}*	{ yylval.s = yytext; return NAME; }

[^ \t\n()|;&<>]+	{ yylval.s = yytext; return WORD; }

{WHITESPACE}	;
