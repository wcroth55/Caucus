
/*** PROT_xxx    Handle the protection stack.
/
/        prot_push (n)      Push "n" onto protection stack
/    v = prot_top()         Return current value on top of protection stack
/        prot_pop()         Pop one value off of protection stack
/ 
/    Parameters:
/       int      n;       value to push
/       int      v;       value returned from top of stack
/
/    Purpose:
/       Provide an encapsulated way of pushing a value onto the
/       "protection" stack.  This is a stack of values that
/       controls whether the effect of the $protect() function
/       is currently active (see CML Reference guide).
/
/    How it works:
/
/    Home:  sweb/prot.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/30/01 11:36 New function. */

#include <stdio.h>
#include "sweb.h"

#define  MAXPROT   40
static  protstack[MAXPROT] = {0};
static  top = 0;

FUNCTION int prot_top() {
   return protstack[top];
}

FUNCTION void prot_push (int n) {
   if (top < MAXPROT-1)   protstack[++top] = n;
   return;
}

FUNCTION void prot_pop() {
   if (top > 0)  --top;
}
