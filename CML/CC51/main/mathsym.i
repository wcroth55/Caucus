#
#---mathsym.i
#
#   Define "pop-up" (DHTML) symbol table.
#
#   inc(1)  immediately expand symbol macro, else just insert macro
#           for later expansion.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 12/07/04 Modify to use new, improved, popOver() function.
#:CR 01/10/05 Add inc(1)
#-------------------------------------------------------------------------

#---Width of pop-up symbol table
set symwidth 10

#---Function to "pop-up" (make visible) and close (hide) symbol table.
"<script src="/~$caucus_id()/popover.js"></script>
"<script type="text/javascript" language="javascript">
"  var visible = false;
"  function flipSymbols() {
"     visible = ! visible;
"     return popOver ('symbols', (visible ? true : false), 10, -180);
"  }
"</script>


#---Definition/layout of table.
"<div id="symbols" style="POSITION: absolute; visibility: hidden">
"<table bgcolor="#FFFFCC" border=2 cellspacing=0 cellpadding=0>
   "<tr><td><table bgcolor="#FFFFCC" border=0 cellspacing=0 cellpadding=0 >
#  "          width="100%">
      "<tr><td><table bgcolor="#FFFFCC" border=0 cellspacing=2 cellpadding=0 
      "         width="100%">
         set count -1
         for x in $(symnam)
            set count $plusmod ($(count) 1 $(symwidth))
            "$ifthen ($not($(count)) <tr valign=middle>)
            "<td>%mathsym($(x) $inc(1))</td>
         end
#        "<tr>
#        count x 1 $(symwidth)
#           "<td>&nbsp;</td>
#        end
      
      "</table></td>
      "<td valign=top align=right
      "  ><a href="#" onClick="return flipSymbols();"
      "  ><img src="/~$caucus_id()/GIF50/x4.gif" border=0></a></td>
      "</td>
   "</table></td>
"</table>
"</div>

#-------------------------------------------------------------------------
