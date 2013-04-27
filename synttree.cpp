#include <stdio.h>
#include "synttree.h"
#include "symtab.h"

void Error (char *format, ...);

// Node names        
char *name[] = {
   "statement list",
   "empty statement",
   "expression statement",
   "print statement",
   "input statement",
   "if..then statement",
   "if..then..else statement",
   "error statement",

   "equals",
   "assign",
   "concatenate",
   "identifier",
   "string constant",
   "coercion to string"
};

// Numbers of children per node type
int children[] = {
   2, 0, 1, 1, 0, 2, 3, 0, 2, 1, 2, 0, 0, 1
};

// Recursively show the contents of the syntax tree
void TreeNode::Show (int level)   {
   int i,nl;
   if (!this)  return;
   if (type != STMT_LIST)   {
      for (i = 0; i < level; i++)   printf ("   ");
      printf ("%s", name[type]);
      switch (type)  {
      case INPUT_STMT: case ASSIGN_EXPR: case IDENT_EXPR:
         printf (" (%s)", symbol->name); break;
      case STR_EXPR:
         printf (" (\"%s\")", symbol->cont); break;
      }
      nl = level + 1;
      printf ("\n");
   } else nl = level;
   for (i = 0; i < children[type]; i++)   child[i]->Show(nl);
}

// ---------------------- Semantic Checking -------------------------

// Coerce one of the children to string type
int TreeNode::CoerceToString (int childno)  {
   if (child[childno]->rettype == T_STRING)
      return 1;
   if (child[childno]->rettype != T_BOOL)
      return 0;
   child[childno] = new TreeNode (COERCE_TO_STR, child[childno]);
   return 1;
}

// Check the semantics of this node
void TreeNode::Check ()  {
   // First, set the type of value the node 'returns'
   switch (type)  {
   case STMT_LIST: case EMPTY_STMT: case EXPR_STMT:
   case PRINT_STMT: case INPUT_STMT: case IFTHEN_STMT:
   case IFTHENELSE_STMT: case ERROR_STMT:
      rettype = T_VOID;  // statements have no value
      break;
   case EQUAL_EXPR:
      rettype = T_BOOL;
      break;
   case CONCAT_EXPR:
   case ASSIGN_EXPR:
      rettype = T_STRING;
      break;
   case IDENT_EXPR:
   case STR_EXPR:
      rettype = T_STRING;
      break;
   case COERCE_TO_STR:
      rettype = T_STRING;
   }

   // Now, check the semantics
   switch (type)  {
   case IFTHEN_STMT:
   case IFTHENELSE_STMT:
      if (child[0]->rettype != T_BOOL)
         Error ("if: Condition should be boolean");
      break;
   case EQUAL_EXPR: // no coercions here, types have to be equal
      if (child[0]->rettype != child[1]->rettype)
         Error ("==: Different types");
      break;
   case CONCAT_EXPR: // coerce both expressions to string
      // Note: these error messages should never appear
      if (!CoerceToString (0))   
         Error ("+: Can't coerce first argument to string");
      if (!CoerceToString (1))   
         Error ("+: Can't coerce second argument to string");
   case ASSIGN_EXPR: // coerce expression to string
      if (!CoerceToString (0))   
         Error ("=: Can't coerce to string");
      break;
   }
}
