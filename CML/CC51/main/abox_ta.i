#
#---abox_ta.i   Edit box, using <textarea>.
#
#   inc(1)   type of response text (format literal html cml, etc)
#   inc(2)   name of variable containing (existing) text
#   inc(3)   basename of help file for "get Help" link.  E.g. i_item, i_resp.
#   inc(4)   base for uploaded files directory.
#
#   Assumes textbox appears inside a table (the famous new curved-blue-box!)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  4/08/04 New file, hacked-up version of ebox_ta.i.
#: CR  4/27/04 Modify to use popup file attach dialog.
#: CR 01/14/05 Include "use richtext" link.
#: CR 02/10/06 Add inc(4) for generic (not just item/resp) file uploads.
#----------------------------------------------------------------------------

   "<script type="text/javascript" language="javascript">
   "  function ebox_onload (keywords) {
   "  }
   "  function ebox_addtext (theText) {
   "     document.ebox.tx_resp.value += theText;
   "  }
   "  function ebox_addnewline () {
   "     document.ebox.tx_resp.value += '\n';
   "  }
   "  function ebox_texttype (theType) {
   "     document.ebox.tp_resp.selectedIndex = 2;
   "  }
   "</script>

   #---Enter your (whatever) text, or upload a file.
   "<tr $(eboxbg)><td>&nbsp;</td>
   "<td colspan=2><table cellspacing=0 cellpadding=0 border=0>
      "<TR>
      "<TD><img $(src_bullet)>&nbsp;Edit&nbsp;your&nbsp;</td>
      "<TD>
         include $(main)/resptype.i   tp_resp 1 $inc(1)
      "</TD>
      "<TD>text&nbsp;below&nbsp;&nbsp;&nbsp;&nbsp;</TD>
   "</table></td>


   "<td><table cellspacing=0 cellpadding=0 border=0>
      "<tr valign=middle>
      "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</TD>

      "<TD valign=bottom><a href="$(mainurl)/$(uploadPage)$(xuploadLicense).cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$inc(1)+$inc(4)"
      "   onClick="openNearby('attach', 500, 300, \
             'resizable=1,scrollbars=1,toolbar=0');   return true;"
      "    target="attach"><img $(src_upload) border=0></a></td>



#     "<TD valign=bottom><INPUT TYPE="image" NAME="upload" BORDER=0 $(src_upload) 
#     "     HSPACE=3></TD>
      "<TD>&nbsp;a&nbsp;file&nbsp;&nbsp;&nbsp;</TD>
      "<TD>&nbsp;&nbsp;<img $(src_bullet)>&nbsp;</TD>
      "<td valign=bottom>
         include $(main)/mathsym.i
      "<a href="#" onClick="return flipSymbols();"
      "    ><img src="/~$caucus_id()/GIF50/bsymbols.gif" border=0
      "  ></a><br><img $(src_clear) width=1 height=1></td>
      "</table></td>

   "<td><table cellspacing=0 cellpadding=0 border=0>
      "<tr valign=top>
      "<TD><nobr>&nbsp;&nbsp;<img $(src_bullet)
      "   >&nbsp;Get&nbsp;<B><A HREF="javascript: \
   pop_up('help', '$(helpurl)/$inc(3).cml?x+x+1');">Help</A></B>.</nobr></TD>

      if $(can_use_richtext)
         "<TD><nobr>&nbsp;&nbsp;<img $(src_bullet)
         "   >&nbsp;<A HREF="$(mainurl)/ebox_switch.cml?$(nch)+$(nxt)+1"
         "   >Use richtext</a></td>
      end
      "</table></td>

   "<td>&nbsp;</td>
   "<td>&nbsp;</td>

   #---Actual textbox.
   "<tr $(eboxbg)>
   "<td>&nbsp;</td>
   "<td colspan=5><TEXTAREA WRAP=virtual NAME="tx_resp" 
   "                ROWS=$(rows) COLS=$(cols)>$t2esc($($inc(2)))</TEXTAREA>

   "<INPUT TYPE="hidden" NAME="unique_box" VALUE="$unique()">
   "</td>
   "<td>&nbsp;</td>
#----------------------------------------------------------------------------
