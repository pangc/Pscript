#include <stdio.h>
#include <iostream>
#include <cstdarg>
#include "lex.h"
#include "lexsymb.h"
#include "parse.h"
// Names of our tokens for displaying them
char *name[] = {
   "IF", "ELSE", "PRINT", "INPUT", "ASSIGN", "EQUAL",
   "CONCAT", "END_STMT", "OPEN_PAR", "CLOSE_PAR", "BEGIN_CS", "END_CS",
   "ID", "STRING",
};
int errors = 0;

// Function used to report errors
void Error (char *format, ...)   {
   va_list args;

   errors++;
   printf ("Line %d: ", lineno);
   va_start(args, format);
   vfprintf(stdout, format, args);
   va_end(args);
   printf("\n");
}

// Show an error count
void ErrorSummary ()  {
   printf ("%d error(s) were found.\n", errors);
}

// Function called by the parser when an error occurs while parsing
// (parse error or stack overflow)
void yyerror(char *msg) {
   Error (msg);
}


// This function is called by the lexer when the end-of-file
// is reached; you can reset yyin (the input FILE*) and return 0
// if you want to process another file; otherwise just return 1.
extern "C" int yywrap(void) {
  return 1;
}
using namespace std;
// The main program: just report all tokens found
int main (int argc, char *argv[])  {
   int token;
/*
   // Set the input stream (either a file from the command line or stdin)
   yyin = NULL;
   if (argc == 2)   {
      yyin = fopen (argv[1], "rt");
   }
   if (yyin == NULL)
      yyin = stdin;

   // Get all tokens and show them on screen
   while ((token = yylex ()) != 0)   {
      cout <<lineno << "Token: ";
      switch (token)   {
      case ID:
	      cout << name[token-IF] << " = " << yylval.str << endl;
		   delete [] yylval.str;
		   break;
      case STRING:
	      cout << name[token-IF] << " = \"" << yylval.str << "\"" << endl;
		   delete [] yylval.str;
		   break;
      case ERROR_TOKEN:
	      cout << "ILLEGAL TOKEN" << endl;
		   break;
      default:
	      cout << name[token-IF] << endl;
         break;
	   }
   }
   return 0;
   */
   yyin = NULL;
   if (argc == 2)
      yyin = fopen (argv[1], "rt");
   if (yyin == NULL)
      yyin = stdin;
  // call the parser
   yyparse();
   ErrorSummary ();
   return errors ? 1 : 0;
   
}

