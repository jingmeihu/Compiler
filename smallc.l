/*
	NAME:smallc.l
	FUNCTION:a flex based lexical analyser. The lexical analyser reads the source codes of SMALLC and separates them into tokens.

	by Hu Jingmei
*/

%{
#include "smallc.tab.h"
#include "def.h"

extern int error_type;

%}

delim      [ \t]
whitespace {delim}+
newline    \n

letter	   [A-Za-z_]
digit      [0-9]
hexdigit   [A-Fa-f0-9]
octdigit   [0-7]
hex        0[Xx]{hexdigit}+
octal      0{octdigit}+
decimal    {digit}+
integer    {decimal}|{hex}|{octal}

%option noyywrap
%option yylineno

TYPE	   int
IF	   if
ELSE	   else 
BREAK	   break
CONT	   continue
FOR	   for
WHILE	   while
STRUCT	   struct
RETURN  return
id	  {letter}({letter}|{digit})*

SEMI	\;
COMMA	\,
LP	\(
RP	\)
LB	\[
RB	\]
LC	\{
RC	\}
DOT	\.

%%
{whitespace}    {}
{newline}       {}


{STRUCT}		{printf("STRUCT: %s\n", yytext);return (STRUCT);}
{RETURN}		{printf("RETURN: %s\n", yytext);return (RETURN);}
{IF}			{printf("IF: %s\n", yytext);return (IF);}
{ELSE}			{printf("ELSE: %s\n", yytext);return (ELSE);}
{BREAK}		{printf("BREAK: %s\n", yytext);return (BREAK);}
{CONT}			{printf("CONTINUE: %s\n", yytext);return (CONT);}
{FOR}			{printf("FOR: %s\n", yytext);return (FOR);}
{WHILE}		{printf("WHILE: %s\n", yytext);return (WHILE);}
{TYPE}			{
				printf("TYPE: %s\n", yytext);
				char* p = (char*)malloc(strlen(yytext) + 1);
				strcpy(p, yytext);
				yylval.type_str = p;
				return TYPE;
			}

{LP}		{printf("LP:%s\n", yytext);return (LP);}
{RP}		{printf("RP: %s\n", yytext);return (RP);}
{LB}		{printf("LB: %s\n", yytext);return (LB);}
{RB}		{printf("RB: %s\n", yytext);return (RB);}
{LC}		{printf("LC: %s\n", yytext);return (LC);}
{RC}		{printf("RC: %s\n", yytext);return (RC);}
{SEMI}		{printf("SEMI: %s\n", yytext);return (SEMI);}
{COMMA}	{printf("COMMA: %s\n", yytext);return (COMMA);}
{DOT}		{printf("DOT: %s\n", yytext);return (DOT);}

{octal}	{
		printf("INTEGER: %s\n", yytext);
		sscanf(yytext + 1, "%o", &yylval.type_int);
		return (INT);
	}
{decimal}	{
			printf("INTEGER: %s\n", yytext);
			yylval.type_int = atoi(yytext);
			return INT;
		}
{hex}	{
		printf("INTEGER: %s\n", yytext);
		sscanf(yytext + 2, "%x", &yylval.type_int);
		return INT;
	}

"++"		{printf("UNARYOP: %s\n", yytext);return (INCR);}
"--"		{printf("UNARYOP: %s\n", yytext);return (DECR);}
"<<="		{printf("BINARYOP: %s\n", yytext); return (LSHIFTASSIGN);}
">>="		{printf("BINARYOP: %s\n", yytext); return (RSHIFTASSIGN);}
"<<"		{printf("BINARYOP: %s\n", yytext); return (LSHIFT);}
">>"		{printf("BINARYOP: %s\n", yytext); return (RSHIFT);}
"&&"		{printf("BINARYOP: %s\n", yytext); return (LOGIAND);}
"||"		{printf("BINARYOP: %s\n", yytext); return (LOGIOR);}
"+="		{printf("BINARYOP: %s\n", yytext); return (ADDASSIGN);}
"-="		{printf("BINARYOP: %s\n", yytext); return (MINUSASSIGN);}
"*="		{printf("BINARYOP: %s\n", yytext); return (MULTASSIGN);}
"/="		{printf("BINARYOP: %s\n", yytext); return (DIVASSIGN);}
"&="		{printf("BINARYOP: %s\n", yytext); return (BITANDASSIGN);}
"^="		{printf("BINARYOP: %s\n", yytext); return (BITXORASSIGN);}
"|="		{printf("BINARYOP: %s\n", yytext); return (BITORASSIGN);}
">"|"<"|">="|"<="|"=="|"!="	{
					printf("BINARYOP: %s\n", yytext); 
					if (yytext[0] == '>')
					{
						if (yytext[1] == '=')
							yylval.type_rel = R_NOLESSTHAN;
						else
							yylval.type_rel = R_GREATER;
					}
					else if (yytext[0] == '<')
					{
						if (yytext[1] == '=')
							yylval.type_rel = R_NOGREATERTHAN;
						else
							yylval.type_rel = R_LESS;
					}
					else if (yytext[0] == '!')
						yylval.type_rel = R_NOTEQUAL;
					else
						yylval.type_rel = R_EQUAL;
					return (RELOP);
				}
"="			{printf("BINARYOP: %s\n", yytext); return (ASSIGN);}
"*"			{printf("BINARYOP: %s\n", yytext); return (MULT);}
"/"			{printf("BINARYOP: %s\n", yytext); return (DIV);}
"%"			{printf("BINARYOP: %s\n", yytext); return (MOD);}
"+"			{printf("BINARYOP: %s\n", yytext); return (ADD);}
"-"			{printf("BINARYOP: %s\n", yytext); return (MINUS);}
"&"			{printf("BINARYOP: %s\n", yytext); return (BITAND);}
"^"			{printf("BINARYOP: %s\n", yytext); return (BITXOR);}
"|"			{printf("BINARYOP: %s\n", yytext); return (BITOR);}
"!"			{printf("UNARYOP: %s\n", yytext); return (LOGINOT);}
"~"			{printf("UNARYOP: %s\n", yytext); return (BITNOT);}

{letter}({letter}|{digit})*		{
					printf("ID: %s\n", yytext);
					char* p = (char*)malloc(strlen(yytext) + 1);
					strcpy(p, yytext);
					yylval.type_str = p;
					return ID;
				}
%%
