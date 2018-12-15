#
#---showBreakoutLink.i    Show the little "breakout box"s with the
#      links to this current item's child breakouts.
#
#   Input:
#      inc(1) conf num
#      inc(2) item id
#      inc(3) resp num
#      inc(4) if 1, show explanation
#
#:CR 04/22/2007 New file
#:CR 06/26/2008 Don't show breakout links for hidden/retired/forgotten
#--------------------------------------------------------------------

set bExplain  0$inc(4)

set bOrder
set isBlog 0
if $tablefind (x$(type) xfreeblog xmodblog)
   set bOrder DESC
   set isBlog 1
end

set bquery \
    SELECT i.title breakTitle, i.id bId, i.qkey bQkey \
      FROM (items i, conf_has_items h) \
      LEFT JOIN resps_seen s \
        ON (s.userid='$userid()' AND s.items_id = i.id) \
     WHERE i.id = h.items_id \
       AND h.cnum = $inc(1) AND h.items_parent=$inc(2) \
                            AND h.resps_parent=$inc(3) \
       AND h.hidden =0      AND h.retired = 0          \
       AND h.deleted=0 \
       AND (s.forgot IS NULL  OR  s.forgot = 0) \
     ORDER BY h.item0, h.item1 $(bOrder), h.item2, h.item3, h.item4, h.item5
set hb $sql_query_open ($(bquery))
while  $sql_query_row  ($(hb))

   if $(bExplain)
      "<p/>
      "<table cellspacing=0 cellpadding=0 width="70%">
      "<tr>
      "<td>This item also has some sub-items or sub-quizzes which you
      "can navigate to when you are finished with this item.&nbsp;
      "You do not have to follow these links, they are just here
      "for your convenience.</td>
      "</table>
      set bExplain 0
   end

   set breakLabel $item_label ($inc(1) $(bId))
   "<table border="0" cellspacing="1" cellpadding="4" 
   "   bgcolor="#006B91"
   "   style="margin-top: 5px;">
   "<tr><td bgcolor="#CEEAF5"
   "><font face="Arial,Helvetica,sans-serif" 
   " style="font-family: Arial, Helvetica, sans-serif; font-size: 8pt;">
   if $not ($(isBlog))
      "Breakout: see
   end
   if $empty($(bQkey))
      "<A HREF="viewitem.cml?$(nch)+$(nxt)+$inc(1)+$(bId)+0+x+x+x" 
   end
   else
      "<A HREF="quiz_take.cml?$(nch)+$(nxt)+$inc(1)+$(bId)+x+x+x+x"
   end
   "    >$(breakLabel) $(breakTitle)</a>
   "</font></td>
   "</table>
end
eval $sql_query_close($(hb))
