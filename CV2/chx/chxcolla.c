
/*** CHXCOLLATE.  Set or use chix collating table.
/
/    v = chxcollate (verb, a, b, c, i);
/
/    Parameters:
/       int4  v;     (collating value of A if verb==APPLY)
/       int   verb;  (CLEAR_CT, ADDRULE_CT, APPLY_CT)
/       int4  a;     (first char in range; or char to APPLY rules to)
/       int4  b;     (last  char in range)
/       int4  c;     (transformed value of A)
/       int4  i;     (transform increment value)
/
/    Purpose:
/       Chix applications generally require some sort of collating order
/       of chix characters.  Chxcollate() encapsulates all information about
/       such a collating order.  It allows for the definition of an arbitrary
/       function mapping between a character's value as an n-byte integer,
/       and its collating value.
/
/       The only restriction on this mapping is it must be 1-to-1: different
/       characters must NOT have the same collating value.
/
/       Chxcollate() is called to initialize, set, and apply said collating
/       order, depending on the value of argument VERB.
/
/    How it works:
/       The verb CLEAR_CT means "clear the collating order".  This means that
/       the collating value of a character is simply the value of the character.
/       Any existing rules are erased.
/
/       ADDRULE_CT means "add a rule to the collating order".  Rules are of the
/       form (a, b, c, i).  A rule means that:
/           value A   transforms to value C
/                 A+1 transforms to       C + I
/                 A+2      "     "        C + 2*I
/                 ...                     ...
/                 B                       C + n*I
/
/       APPLY_CT means "apply the list of rules, in order (of definition)", to
/       character value A, and return the result.
/
/    Returns: for CLEAR_CT and ADDRULE_CT: 0L.
/             for APPLY_CT: the collating value of A.
/             for errors: -1L.
/
/    Error Conditions: bad VERB.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application, chxcompare()
/
/    Operating system dependencies: depends on sysfree().
/
/    Known bugs:      none
/
/    Home:  chx/chxcolla.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */

#include <stdio.h>
#include "chixdef.h"

#define  RNULL  ((struct rule_t *) NULL)

static struct rule_t  head  = {0L, 0L, 0L, 0L, RNULL};
static struct rule_t *rules = &head;

FUNCTION  int4 chxcollate (verb, a, b, c, i)
   int    verb;
   int4   a, b, c, i;
{
   struct rule_t *p, *q;
   int4           value;

   ENTRY ("chxcollate", "");

   /*** The table of rules is kept as a linked list.  Each time a rule
   /    is added, it goes onto the end of the list.  Rules are applied
   /    in order, from the start of the list through to the end. */
   value = 0L;

   /*** Clear the rules table by freeing all elements of the linked list. */
   if (verb == CLEAR_CT) {
      for (p=rules->next;   p!=RNULL;  ) {
         q = p;
         p = p->next;
         sysfree ( (char *) q);
      }
      rules->next = RNULL;
   }

   /*** Add a rule by allocating a new struct rule_t, filling it in, and
   /    putting it on the end of the linked list. */
   else if (verb == ADDRULE_CT) {
      for (p=rules;   p->next != RNULL;   p = p->next)  ;
      q = (struct rule_t *) chxmem (SIZE (head), "chxcollate newrule");
      q->a = a;    q->b = b;    q->c = c;    q->i = i;
      q->next = RNULL;
      p->next = q;
   }

   /*** Apply the rules by applying each transform (that A fits in) to A.
   /    Last rule wins. */
   else if (verb == APPLY_CT  &&  a == 0L)   value = 0L;
   else if (verb == APPLY_CT) {
      value = a;
      for (p=rules->next;   p!=RNULL;   p=p->next) {
         if (a < p->a  ||  a > p->b)  continue;
         value = p->c + p->i * (a - p->a);
      }
   }

   else value = -1L;

   RETURN (value);
}
