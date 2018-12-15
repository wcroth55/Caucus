#
#---TEXTPAGE.I    Show the text-only version of an item
#
#   inc(1)  4-digit conf num
#   inc(2)  item num
#   inc(3)  page num
#   inc(4)  pages
#   inc(5)  name of variable containing list of responses
#   inc(6)  next item (after inc(2))
#
#   Show PAGE of PAGES of conf inc(1), item inc(2).
#   Assumes list of responses appears in variable RLISTn,
#   where 'n' is the page number.
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/26/99 09:51 TSNAP stuff
#: CR 12/16/01 18:38 use m_cnum, m_inum, m_rnum
#----------------------------------------------------------

set fpnum $mac_expand (%digit4($inc(3)))
set finum $mac_expand (%digit4($inc(2)))

eval $output ($(target)/temp)

#"<HTML>
#"<HEAD>
set cname %displayed_conf_name ($inc(1))
#"<TITLE>$(cname) Item $inc(2)
#" "$t2hbr($re_title($inc(1) $inc(2) 0))"
#"</TITLE>
#include $(dir)/popup.i
#"</HEAD>

set bgcolor $word (1 $site_data ($inc(1) - conf_colors) BGCOLOR="#FFFFFF")
#"<BODY $(bgcolor)>

#set pagename <H2>$(cname) <NOBR>Item $inc(2)</NOBR></H2>
set pagename $(cname) Item $inc(2)
#include $(dir)/headtool.i $inc(1)
#include $(dir)/location.i $inc(1) $inc(2) $word (1 $($inc(5)))

#---Item title and navigation buttons.
#"<TABLE CELLSPACING=0  CELLPADDING=0 WIDTH=100%>
#"<TR>
#  "<TD>
#  "<FONT $(color) SIZE=+2 STYLE=BOLD>
#  "$t2hbr($re_title($inc(1) $inc(2) 0))</FONT>
  "$re_title($inc(1) $inc(2) 0)
# "</TD>

# "<TD ALIGN=RIGHT VALIGN=TOP>
  #---Navigation-within-item buttons.
# include $(dir)/navigate.i $inc(1) $(finum) $inc(3) $inc(4)
# "</TD>
#"</TABLE>

#"<P>
"
set rlast $word ($sizeof($($inc(5))) $($inc(5)))
for response in $($inc(5))
   set m_rnum $(response)
   include $(dir)/textresp.i $inc(1) $inc(2) $(response) $equal ($(response) $(rlast))
end
#"<P>

#"<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0>
#"<TR>
if $and ($inc(3)  $less ($inc(3) $(pages)) )
#   "<TD COLSPAN=4>
#     "<B>There are more responses to this item on the next page</B>
      "There are more responses to this item on the next page
#   "</TD>
#   "<TD ALIGN=RIGHT>
#     include $(dir)/navigate.i $inc(1) $(finum) $inc(3) $inc(4)
#   "</TD>
end
else
#   "<TD  ALIGN=right>
#      "<TABLE>
#      "<TR>
      if $empty ($inc(6))
#         "<TD><B>On to $(cname) $(home):&nbsp;</B></TD>
#         "<TD><A HREF="../C$inc(1)/index.htm"><IMG $(src_pass) BORDER=0></A></TD>
      end
      else
         set fnext $mac_expand (%digit4($inc(6)))
#         "<TD><B>On to next item:&nbsp;</B></TD>
#         "<TD><A HREF="../C$inc(1)/$(fnext)0001.htm"
#         "   ><IMG $(src_pass) BORDER=0></A></TD>
      end
#      "</TABLE>
#   "</TD>
end
#"</TABLE>
#"<P>
#"<HR>

#"</BODY>
#"</HTML>
eval $output()

eval $copyfile ($(target)/temp $(target)/T$inc(1)/$(finum)$(fpnum).txt 755)
#-----------------------------------------------------------------------
