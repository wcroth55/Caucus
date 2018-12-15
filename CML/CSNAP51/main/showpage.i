#
#---SHOWPAGE.I    Show a page of an item
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
#: CR 12/16/01 18:38 use m_cnum, m_inum, m_rnum
#----------------------------------------------------------

set fpnum $mac_expand (%digit4($inc(3)))
set finum $item_label($inc(1) $inc(2))

set localerrtext
eval $output ($(target)/temp)

"<HTML>
"<HEAD>
set cname %displayed_conf_name ($inc(1))
"<TITLE>$(cname) Item $(finum)
" "$t2hbr($(title))"
"</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

set bgcolor $word (1 $site_data ($inc(1) - conf_colors) BGCOLOR="#FFFFFF")
"<BODY $(bgcolor)>

set pagename <H2>$(cname) <NOBR>Item $(finum)</NOBR></H2>
include $(dir)/headtool.i $inc(1)
include $(dir)/location.i $inc(1) $(finum) $word (1 $($inc(5)))

#---Item title and navigation buttons.
"<TABLE CELLSPACING=0  CELLPADDING=0 WIDTH=100%>
"<TR>
  "<TD><span class="item_title">$t2hbr($(title))</span
  "   ></TD>

  "<TD ALIGN=RIGHT VALIGN=TOP>
  #---Navigation-within-item buttons.
  include $(dir)/navigate.i $inc(1) $inc(2) $inc(3) $inc(4)
  "</TD>
"</TABLE>

"<p/>
      set q SELECT i.title, i.lastresp, i.userid, u.fname, u.lname \
              FROM items i \
              LEFT JOIN user_info u \
                ON (u.userid = i.userid) \
             WHERE i.id=$(inum)

set rlast $word ($sizeof($($inc(5))) $($inc(5)))
set q SELECT r.rnum, r.text, r.userid, r.prop, r.time, u.fname, u.lname, \
             r.copy_rnum, r.copy_item, r.copy_cnum, r.copier, r.copy_time, r.copy_show \
        FROM resps r \
        LEFT JOIN user_info u \
          ON (u.userid = r.userid) \
       WHERE r.items_id = $inc(2) AND r.rnum IN ($commaList(n $($inc(5)))) \
         AND r.deleted=0 AND r.version =  \
             (SELECT MAX(q.version) FROM resps q \
               WHERE q.items_id=$inc(2) AND q.rnum = r.rnum) \
       ORDER BY r.rnum

set h $sql_query_open($(q))
while $sql_query_row ($(h))
   set m_rnum   $(rnum)
   set response $(rnum)
   include $(dir)/showresp.i $inc(1) $inc(2) $(response) $equal ($(response) $(rlast))
   if $(localerrs)
      set errtext $errors()
      if $not_empty ($(errtext))
         set localerrtext $(localerrtext) Resp=$(response) $(errtext)$newline()
      end
   end
end
eval $sql_query_close($(h))
      
"<p/>

"<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0>
"<TR>
if $and ($inc(3)  $less ($inc(3) $(pages)) )
   "<TD COLSPAN=4>
      "<B>There are more responses to this item on the next page</B>
   "</TD>
   "<TD ALIGN=RIGHT>
     include $(dir)/navigate.i $inc(1) $inc(2) $inc(3) $inc(4)
   "</TD>
end
else
   "<TD  ALIGN=right>
      "<TABLE>
      "<TR>
      if $empty ($inc(6))
         "<TD><B>On to $(cname) $(home):&nbsp;</B></TD>
         "<TD><A HREF="../C$inc(1)/index.htm"><IMG $(src_pass) BORDER=0></A></TD>
      end
      else
         set fnext $item_label($inc(1) $inc(6))
         "<TD><B>On to next item:&nbsp;</B></TD>
         "<TD><A HREF="../C$inc(1)/$inc(6)0001.htm"
         "   ><IMG $(src_pass) BORDER=0></A></TD>
      end
      "</TABLE>
   "</TD>
end
"</TABLE>
"<P>

"</BODY>
"</HTML>
eval $output()
if $not_empty ($(localerrtext))
   "$(localerrtext)
end

eval $copyfile ($(target)/temp $(target)/C$inc(1)/$inc(2)$(fpnum).htm 755)
#-----------------------------------------------------------------------
