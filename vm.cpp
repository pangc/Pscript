// VM.CPP: The virtual machine
#include <iostream>
#include <string.h>
#include "symtab.h"
#include "vm.h"       // VMachine class definition

// VMachine constructor
VMachine::VMachine ()   {
   instr = NULL; ninstr = 0;
   int i;
   for (i=0; i<MAX_STR; i++)   
      str[i]=NULL;
}

VMachine::~VMachine () {
   Reset();
}

// Reset the virtual machine
void VMachine::Reset ()   {
   int i;
   for (i=0; i<MAX_STR; i++)    {
      if (str[i] != NULL)   {delete str[i]; str[i] = NULL;}
   }
   if (instr != NULL)  {delete[] instr; instr = NULL; ninstr = 0;}
   stack.Empty();
}

extern SymTab st;
extern IntInstr *intcode;

// Read a program
// (connected to the compiler for extreme laziness purposes)
void VMachine::Read ()   {
   int i=0;

   SymDesc *s = st.GetFirst();
   while (s != NULL)   {
      if (s->cont != NULL)
         str[i] = new String (s->cont);
      else 
         str[i] = new String();
      s->SetNo (i); // Set number so we can find its index back later (aarggh.. dirty coding!!)
      i++;
      s = st.GetNext();
   }

   ninstr = intcode->Len();
   instr = new Instr[ninstr];
   IntInstr *cinstr = intcode;
   for (i = 0; i < ninstr; i++)   {
      switch (cinstr->opcode)   {
      case OP_NOP:           // no operation
         instr[i] = Instr (OP_NOP, 0);
         break;
      case OP_PUSH:          // push string [var]
         instr[i] = Instr (OP_PUSH, cinstr->str->GetNo());
         break;
      case OP_GETTOP:        // get string from top of stack (=assign) [var]
         instr[i] = Instr (OP_GETTOP, cinstr->str->GetNo());
         break;
      case OP_DISCARD:       // discard top value from the stack
         instr[i] = Instr (OP_DISCARD);
         break;
      case OP_PRINT:         // print a string
         instr[i] = Instr (OP_PRINT);
         break;
      case OP_INPUT:         // input a string [var]
         instr[i] = Instr (OP_INPUT, cinstr->str->GetNo());
         break;
      case OP_JMP:           // unconditional jump [dest]
         instr[i] = Instr (OP_JMP, cinstr->target->n - i);
         break;
      case OP_JMPF:          // jump if false [dest]
         instr[i] = Instr (OP_JMPF, cinstr->target->n - i);
         break;
      case OP_STR_EQUAL:     // test whether two strings are equal
         instr[i] = Instr (OP_STR_EQUAL);
         break;
      case OP_BOOL_EQUAL:    // test whether two bools are equal
         instr[i] = Instr (OP_BOOL_EQUAL);
         break;
      case OP_CONCAT:        // concatenate two strings
         instr[i] = Instr (OP_CONCAT);
         break;
      case OP_BOOL2STR:      // convert bool to string
         instr[i] = Instr (OP_BOOL2STR);
         break;
      case JUMPTARGET:       // not an opcode but a jump target;
         instr[i] = Instr (OP_NOP);
         break;
      }
      cinstr = cinstr->next;
   }
}

// Execute the program in memory
void VMachine::Execute ()   {
   int ip; // instruction pointer
   VM_Stack stack;  // the stack
   int ipc; // instruction pointer change
   int i,j,k;

   ip = 0; // start at instruction 0
   while (ip < ninstr)   {
      ipc = 1; // default: add one to ip
      switch (instr[ip].opcode)   {
      case OP_NOP:
         // No OPeration
         break;
      case OP_PUSH:
         // Push a variable onto the stack
         stack.Push (NewTempCopy(instr[ip].operand));
         break;
      case OP_GETTOP:
         str[instr[ip].operand]->Assign (*str[stack.GetTop()]);
         break;
      case OP_DISCARD:
         DelTempCopy (stack.Pop());
         break;
      case OP_PRINT:
         i = stack.Pop();
         str[i]->Print();
         DelTempCopy (i);
         break;
      case OP_INPUT:
         str[instr[ip].operand]->Input ();
         break;
      case OP_JMP:
         ipc = instr[ip].operand;
         break;
      case OP_JMPF:
         i = stack.Pop();
         if (i == ST_FALSE)   ipc = instr[ip].operand;
         break;
      case OP_STR_EQUAL:
         i = stack.Pop(); j = stack.Pop();
         if (strcmp (str[i]->Val(), str[j]->Val()) == 0)   k = ST_TRUE; else k = ST_FALSE;
         DelTempCopy (i); DelTempCopy (j);
         stack.Push (k);
         break;
      case OP_BOOL_EQUAL:
         i = stack.Pop(); j = stack.Pop();
         if (i == j)   k = ST_TRUE; else k = ST_FALSE;
         stack.Push (k);
         break;
      case OP_CONCAT:
         i = stack.Pop(); j = stack.Pop();
         k = NewTempCopy (j); str[k]->Concatenate (*str[i]);
         DelTempCopy (i); DelTempCopy (j);
         stack.Push (k);
         break;
      case OP_BOOL2STR:
         i = stack.Pop();
         if (i == ST_FALSE)   i = NewTempCopy ("false"); else i = NewTempCopy("true");
         stack.Push (i);
         break;
      }
      ip += ipc;
   }
}

// Returns the index to a new temp. string
int VMachine::NewTempCopy ()  {
   int i;

   for (i=0; i < MAX_STR; i++)   {
      if (str[i] == NULL)  {
         str[i] = new String;
         break;
      }
   }
   if (i == MAX_STR)
      return -1;
   else return i;
}

// Returns the index to a new copy of string j
int VMachine::NewTempCopy (int j)  {
   int i = NewTempCopy();
   if (i>=0)  str[i]->Assign (*str[j]);
   return i;
}

// Returns the index to a new copy of string s
int VMachine::NewTempCopy (char *s)  {
   int i = NewTempCopy ();
   if (i>=0)  str[i]->Assign (s);
   return i;
}

// Deletes the previously generated temp. string
void VMachine::DelTempCopy (int i)  {
   if (str[i] != NULL)   {delete str[i]; str[i] = NULL;}
}

