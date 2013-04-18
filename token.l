/* Lexical analyzer for StringC */

%{

/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */

// Includes
#include <string.h>   // strcpy, strncpy

#define  _LEX_CPP_   // make sure our variables get created
#include "lex.h"
#include "lexsymb.h"

extern "C" int yywrap (); // the yywrap function is declared by the caller

// Forward references
void Identifier ();
void StringConstant ();
void EatComment ();

//// End of inititial code
%}

/* ------------------------------------------------------------------
   Some macros (standard regular expressions)
   ------------------------------------------------------------------ */

LETTER   [a-zA-Z_]
DIGIT    [0-9]
IDENT    {LETTER}({LETTER}|{DIGIT})*
STR      \"[^\"]*\"
WSPACE   [ \t]+


/* ------------------------------------------------------------------
   The lexer rules
   ------------------------------------------------------------------ */
%%

"if"     {return IF;}
"else"   {return ELSE;}
"print"  {return PRINT;}
"input"  {return INPUT;}
"="      {return ASSIGN;}
"=="     {return EQUAL;}
"+"      {return CONCAT;}
";"      {return END_STMT;}
"("      {return OPEN_PAR;}
")"      {return CLOSE_PAR;}
"{"      {return BEGIN_CS;}
"}"      {return END_CS;}
{IDENT}  {Identifier ();             /* identifier: copy name */
          return ID;}
{STR}    {StringConstant ();         /* string constant: copy contents */
          return STRING;}
"//"     {EatComment();}             /* comment:    skip */
\n       {lineno++;}                 /* newline:    count lines */
{WSPACE} {}                          /* whitespace: (do nothing) */

%%

/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

// The comment-skipping function: skip to end-of-line
void EatComment()  {
	char c;

   while ((c = yyinput()) != '\n' && c != 0);
	lineno++;
}

// Pass the id name
void Identifier ()  {
   yylval.str = new char[strlen(yytext)+1];
   strcpy (yylval.str, yytext);
}

// Pass the string constant
void StringConstant()  {
   int l = strlen(yytext)-2;
   yylval.str = new char[l+1];
   strncpy (yylval.str, &yytext[1], l); yylval.str[l] = 0;
}
