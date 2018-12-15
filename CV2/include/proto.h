/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

int4  ascofchx   (char *s, Chix a, int4 i, int4);
char *ascquick   (Chix x);
int   chxabr     (Chix a, Chix b);
Chix  chxalloc   (int4 maxlen, int type, char *name);
Chix  chxalsub   (Chix a, int4 i, int4 n);
int   chxalter   (Chix a, int4 pos, Chix b, Chix c);
int   chxbreak   (Chix a, Chix fore, Chix aft, Chix pat);
int   chxcat     (Chix a, Chix b);
int   chxcatsub  (Chix a, Chix b, int4 i, int4 n);
int   chxcatval  (Chix a, int width, int4 v);
int   chxclear   (Chix x);
int4  chxcollate (int verb, int4 a, int4 b, int4 c, int4 i);
int   chxcompare (Chix x, Chix y, int4 len);
int   chxconc    (Chix a, Chix b, Chix c);
int   chxcpy     (Chix a, Chix b);
