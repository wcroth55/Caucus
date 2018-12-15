#
#---TABLESHOW.I   Display a Caucus "Dynamic Table".
#
#   inc(1)   tablename
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/12/01 09:30 Dynamic tables.
#============================================================================

if $array_size ($inc(1))
   "<B>Table: %tableedit ($inc(1))</B>
   "<BR>
   "<TABLE BORDER=1>
   "<TR>
   "<TD></TD>

   count col 0 $minus ($array_size ($inc(1) 2) 1)
      "<TD ALIGN=right><B>$(col)</B></TD>
   end

   count row 0 $minus ($array_size ($inc(1) 1) 1)
      "<TR>
      "<TD><B>$(row)<B></TD>

      count col 0 $minus ($array_size ($inc(1) 2) 1)
         "<TD>&nbsp;$array_eval ($inc(1) $(row) $(col))</TD>
      end
   end
   "</TABLE>
end
else
   "<I>No table $inc(1)</I>"<BR>
end
#------------------------------------------------------------------------
