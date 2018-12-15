
/*** FUNC_STACK.   Handle $stack_xxx()  functions.
/
/    func_stack (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Handle stack related functions.
/       Append value of stack_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/       Since many CML pages may "push" something onto a stack, but
/       only some "pop" values off the stack, we use a "circular" stack
/       of a fixed size.
/
/       stack_push(x) pushes "x" onto the stack.
/       stack_pop (y) pops a value off the stack; if the value is empty,
/                     it returns "y" instead.
/
/       The stack is initialized to all slots empty.
/
/    Returns: void
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcstack.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/96 12:33 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  STACKSIZE  25

FUNCTION  func_stack (Chix result, char *what, Chix arg)
{
   static Chix  stack[STACKSIZE] = {nullchix};
   static int   ptr;
   int          slot, down, ptr2;
   int4         pos;
   ENTRY ("func_stack", "");
   
   /*** Initialization. */
   if (stack[0] == nullchix) {
      for  (slot=0;   slot<STACKSIZE;   ++slot)
         stack[slot] = chxalloc (L(60), THIN, "func_stack stack[]");
      ptr = 0;
   }
   
   if (streq (what, "stack_push")) {
      ptr2 = (ptr + 1) % STACKSIZE;
      if (NOT chxeq (stack[ptr], arg)) {
         ptr = ptr2;
         chxcpy (stack[ptr], arg);
      }
   }

   else if (streq (what, "stack_pop")) {
      chxcat   (result, (chxvalue(stack[ptr], L(0)) == 0 ? arg : stack[ptr]));
      chxclear (stack[ptr]);
      ptr = (ptr - 1 + STACKSIZE) % STACKSIZE;
   }

   else if (streq (what, "stack_top")) {
      pos  = L(0);
      down = chxint4 (arg, &pos);
      if (pos == L(0))  down = 0;
      chxcat (result, stack [ (ptr+down+STACKSIZE) % STACKSIZE ]);
   }

   else if (streq (what, "stack_clear")) {
      for (slot=0;   slot < STACKSIZE;   ++slot)  chxclear (stack[slot]);
      ptr = 0;
   }

   RETURN (1);
}
