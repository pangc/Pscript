#ifndef _SYMTAB_H_
#define _SYMTAB_
#include <iostream>
#include <string.h>

enum SymType   {
   STR_VAR,    // String variable
   STR_CONST,  // String constant
};

class SymDesc {
public:
   SymDesc (char *_name, SymType _type, char *_cont, int _line);
   ~SymDesc ();
   void Show ();

   // index number for VMachine::Read()  (aarggh.. dirty coding!!)
   void SetNo (int n)  {no=n;}
   int  GetNo ()    {return no;}
   int no;

   char    *name;   // name of the symbol
   char    *cont;   // contents (if string constant)
   SymType  type;   // type of the symbol
   int      line;   // line it was first encountered
   SymDesc *next;   // next symbol in list
};

class SymTab  {
public:
   SymTab();
   ~SymTab();
   bool Add (SymDesc *symb);
   SymDesc *Find (char *name);
   void Show ();
   SymDesc *GetFirst ()  {current = start; return current;}
   SymDesc *GetNext ()   {
      if (current != NULL)
         current=current->next; 
      return current;
   }

private:
   SymDesc *start;
   SymDesc *current;
};

#endif 