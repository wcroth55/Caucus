#
#---TABLEMAC.I   Caucus "Dynamic Table" specific macros.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/12/01 09:30 Dynamic tables.
#: CR 10/11/05 C5 changes.
#============================================================================

#---Display a table(!)
eval $mac_define (table \$includecml ($(main)/tableshow.i @1))

#---Edit a table:  %tableedit (tablename)
eval $mac_define (tableedit \
       \$if (\$less (\$group_access (\$userid() Caucus4 edit@1) $priv(minimum)) @1 \
           <A HREF="#" onClick="return tablepop('@1');">@1</A> \
       ) \
     )

#---Sum of cell range, e.g. %sum(0 R0  9 R0) is sum of 1st 10 rows, this column.
eval $mac_define (sum      \$plus (\$cells (@@)))

#---Sum of all cells (in row) to the left of this one.
eval $mac_define (sumleft  \$plus (\$cells (R0  0   R0  R-1)))

#---Sum of all cells (in column) above this one.
eval $mac_define (sumabove \$plus (\$cells ( 0 R0   R-1 R0 )))

#===============================================================
