#include <stdio.h>
#include "symtab.h"
#include "synttree.h"
#include "intcode.h"

// Number: assigns line numbers to this block of intermediate code, starting with ln
void IntInstr::Number (int ln)   {
   IntInstr *num = this;
   while (num != NULL)   {
      num->n = ln++; num = num->next;
   }
}

// Names of the opcodes
char *op_name[] = {
   "OP_NOP", "OP_PUSH", "OP_GETTOP", "OP_DISCARD",
   "OP_PRINT", "OP_INPUT", "OP_JMP", "OP_JMPF",
   "OP_STR_EQUAL", "OP_BOOL_EQUAL", "OP_CONCAT",
   "OP_BOOL2STR", "JUMPTARGET"
};

// Show this block of intermediate code
void IntInstr::Show ()   {
   printf ("%2d: %s ", n, op_name[opcode]);
   if (str)     printf ("%s ", str->name);
   if (target)  printf ("%d", target->n);
   printf ("\n");
   if (next != NULL)   next->Show();
}

// Concatenates two blocks of instructions
IntInstr *Concatenate (IntInstr *blk1, IntInstr *blk2)  {
   IntInstr *search = blk1;
   while (search->next != NULL)   search = search->next;
   search->next = blk2;
   return blk1;
}

// Prefix a jump target to a block; returns the new block
IntInstr *PrefixJT (IntInstr *blk, IntInstr *ref_instr)   {
   IntInstr *jt = new IntInstr;
   jt->opcode = JUMPTARGET;
   jt->target = ref_instr;   // the referring instruction
   jt->next = blk;
   return jt;
}

// Recursively generate intermediate code
IntInstr *GenIntCode (SyntTree tree)  {
   TreeNode *root = tree;
   IntInstr *blk1, *blk2,
            *cond, *jump2else, *thenpart, *jump2end, *elsepart, *endif;

   switch (root->type)  {
   case STMT_LIST:
      blk1 = GenIntCode (root->child[0]);
      blk2 = GenIntCode (root->child[1]);
      Concatenate (blk1, blk2);
      return blk1;
   case EMPTY_STMT:
      return new IntInstr (OP_NOP);
   case EXPR_STMT:
      blk1 = GenIntCode (root->child[0]);
      blk2 = new IntInstr (OP_DISCARD);
      return Concatenate (blk1, blk2);
   case PRINT_STMT:
      blk1 = GenIntCode (root->child[0]);
      blk2 = new IntInstr (OP_PRINT);
      return Concatenate (blk1, blk2);
   case INPUT_STMT:
      return new IntInstr (OP_INPUT, root->symbol);
   case IFTHEN_STMT:
      // First, create the necessary code parts
      cond      = GenIntCode (root->child[0]);
      jump2end  = new IntInstr (OP_JMPF);      // set target below
      thenpart  = GenIntCode (root->child[1]);
      endif     = new IntInstr (JUMPTARGET, jump2end);
      jump2end->target = endif;

      // Now, concatenate them all
      Concatenate (cond, jump2end);
      Concatenate (jump2end, thenpart);
      Concatenate (thenpart, endif);
      return cond;
   case IFTHENELSE_STMT:
      // First, create the necessary code parts
      cond      = GenIntCode (root->child[0]);
      jump2else = new IntInstr (OP_JMPF);      // set target below
      thenpart  = GenIntCode (root->child[1]);
      elsepart  = PrefixJT (GenIntCode (root->child[2]), jump2else);
      jump2else->target = elsepart;
      jump2end  = new IntInstr (OP_JMP);       // set target below
      endif     = new IntInstr (JUMPTARGET, jump2end);
      jump2end->target = endif;

      // Now, concatenate them all
      Concatenate (cond, jump2else);
      Concatenate (jump2else, thenpart);
      Concatenate (thenpart, jump2end);
      Concatenate (jump2end, elsepart);
      Concatenate (elsepart, endif);
      return cond;
   case ERROR_STMT:
      return new IntInstr (OP_NOP);
   case EQUAL_EXPR:
      blk1 = GenIntCode (root->child[0]);
      blk2 = GenIntCode (root->child[1]);
      Concatenate (blk1, blk2);
      if (root->child[0]->rettype == T_STRING)
         return Concatenate (blk1, new IntInstr (OP_STR_EQUAL));
      else
         return Concatenate (blk1, new IntInstr (OP_BOOL_EQUAL));
   case ASSIGN_EXPR:
      blk1 = GenIntCode (root->child[0]);
      blk2 = new IntInstr (OP_GETTOP, root->symbol);
      return Concatenate (blk1, blk2);
   case CONCAT_EXPR:
      blk1 = GenIntCode (root->child[0]);
      blk2 = GenIntCode (root->child[1]);
      Concatenate (blk1, blk2);
      return Concatenate (blk1, new IntInstr (OP_CONCAT));
   case IDENT_EXPR:
   case STR_EXPR:
      return new IntInstr (OP_PUSH, root->symbol);
   case COERCE_TO_STR:
      blk1 = GenIntCode (root->child[0]);
      blk2 = new IntInstr (OP_BOOL2STR);
      return Concatenate (blk1, blk2);
   }
   return new IntInstr (OP_NOP); // shouldn't happen
}
