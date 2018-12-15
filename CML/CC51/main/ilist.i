#
#---ILIST.I     List item titles and entry points
#
#   Input:
#     $inc(1)   ilist    name of var containing triplet list of item numbers.
#     $inc(2)   check    1=>include check box for each item
#     $inc(3)   sort     column by which the list should be sorted (sort code):
#                          (0=number, 1=title, 2=author, 3=last)
#     $inc(4)   columns  list of columns to display (quoted)
#     $inc(5)   userid   empty => report new material, uid => report
#                                    on what *that* person has seen.
#     $inc(6)   popup    ilist was called from inside a pop-up window
#     chk                table of values that should have check boxes CHECKED
#     cnum               conference number
#
#   Output: (to browser)
#     List of items, sorted appropriately.
#     After a submit, $form(code) has the newly selected sort code.
#
#   Actions:
#     Clicking on a table column link submits the page to itself,
#     resorting the items according to that column.
#
#   Notes:
#     ilist.i must be enclosed in a <FORM NAME="isort"...>,
#     and a <TABLE>.
#
#     Caller must include setsort.js.
#
#     Columns include:
#        number     item number
#        title      item title
#        author     item author
#        new        indication of new material (icon or # of new responses)
#        last       number of (link to) last response in item
#        last_date  date of last response
#        frozen     is this item frozen or hidden?
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/20/03 Add timezonehrs, timezonename interface variables.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/13/04 Major reformat, using CSS.
#: CR 08/01/05 Add retired.
#: CR 03/02/06 <nobr> names if less than 40 chars
#: CR 02/04/08 Line up breakout item titles on wrap.
#: CR 04/10/09 Properly sort items by title (see ilistTitleSorter.i)
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#: CR 05/07/17 Hack to prevent duplicates of same item from appearing.
#============================================================

#---Table column captions.  Show currently selected "sort by"
#   caption in italics.  Each link is actually a form submission
#   that causes the list of items to be displayed sorted by the
#   title of that link.

#---Item Number
"<TR class="itemlist_header">
if $equal (x$inc(2) x1)
   "<td></td>
end

set who $word (1 $inc(5) $userid())
for col in $unquote($inc(4))

   #---Item number (link to imanage.cml)
   if $equal ($(col) number)
    "<TD class="itemlist_row_num"><B>
    "<A HREF="#" onClick="return setsort(0);" %help(h_sort)>
    "$if($equal($inc(3) 0) <I>)#</I></A
    "></B>&nbsp;</TD>
   end

   #---Title
   if $equal ($(col) title)
    "<td><a href="#" onClick="return setsort(1);" %help(h_sort)>
    "  $if($equal($inc(3) 1) <I>)Title</I></B></A>
    "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    "<SMALL><nobr>(click on column header to sort item list)\
                   &nbsp;&nbsp;</nobr></SMALL></TD>
   end
  
   #---Author
   if $equal ($(col) author)
      "<TD class="itemlist_row_text"
      "  ><B><A HREF="#" onClick="return setsort(2);" %help(h_sort)
      "  >$if($equal($inc(3) 2) <I>)Author</I></A></B></TD>
   end
  
   #---New/Seen
   if $equal ($(col) new)
    "<TD class="itemlist_row_num"><B>$if ($empty($inc(5)) New Seen)</B></TD>
   end
  
   #---Last (Number of last response)
   if $equal ($(col) last)
      "<TD class="itemlist_row_num"
      "  ><B><A HREF="#" onClick="return setsort(3);" %help(h_sort)
      "  >$if($equal($inc(3) 3) <I>)Last</I></A></B></TD>
   end
  
   #---Date of last response
   if $equal($(col) last_date)
      "<TD class="itemlist_row_num"><B>
      "<A HREF="#" onClick="return setsort(3);" %help(h_sort)>
      "  $if($equal($inc(3) 3) <I>)Last Active</I></A>
      "</B></TD>
   end

   #---Frozen?
   if $equal($(col) frozen)
    "<TD class="itemlist_row_text"><B>State&nbsp;&nbsp;</B></TD>
   end

end
"</tr>


#---Prepare comma-separated list of item_ids from triplet in inc(1).
set ids
for cn id rn in $($inc(1))
   set ids $(ids) $(id)
end
set ids $commaList(number $(ids))

#---Prepare sorting order.
set orderBy h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
if   $equal ($inc(3) 2)
   set orderBy u.lname, u.fname, $(orderBy)
end
elif $equal ($inc(3) 3)
   set orderBy r.time DESC
end

#---Row for each item in '$inc(1)' list.
set qkey
set qlist \
    SELECT h.items_id, h.item0, h.item1, h.item2, h.item3, h.item4, h.item5, \
           h.hidden, h.retired, \
           s.seen, s.forgot, u.userid uu, u.fname, u.lname, \
           i.userid, i.title, i.frozen, i.lastresp, r.time, i.qkey, i.type \
      FROM (resps r, items i, conf_has_items h) \
      LEFT JOIN resps_seen s \
        ON (s.userid = '$(who)' AND s.items_id = h.items_id) \
      LEFT JOIN user_info u \
        ON (i.userid = u.userid) \
     WHERE h.cnum=$(cnum) AND h.items_id = i.id \
       AND h.deleted = 0 \
       AND i.id IN ($(ids)) \
       AND r.items_id = i.id  AND r.rnum = i.lastresp  AND r.version= \
           (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
     ORDER BY $(orderBy)

if   $equal ($inc(3) 1)
   include $(main)/ilistTitleSorter.i $(cnum)
   set qlist \
       SELECT h.items_id, h.item0, h.item1, h.item2, h.item3, h.item4, h.item5, \
              h.hidden, h.retired, \
              s.seen, s.forgot, u.userid uu, u.fname, u.lname, \
              i.userid, i.title, i.frozen, i.lastresp, r.time, i.qkey \
         FROM (resps r, items i, conf_has_items h, $(sorter) sr) \
         LEFT JOIN resps_seen s \
           ON (s.userid = '$(who)' AND s.items_id = h.items_id) \
         LEFT JOIN user_info u \
           ON (i.userid = u.userid) \
        WHERE h.cnum=$(cnum) AND h.items_id = i.id \
          AND h.deleted = 0 \
          AND i.id IN ($(ids)) \
          AND r.items_id = i.id  AND r.rnum = i.lastresp AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
          AND sr.i0 = h.item0 AND sr.i1 = h.item1 AND sr.i2 = h.item2 \
          AND sr.i3 = h.item3 AND sr.i4 = h.item4 AND sr.i5 = h.item5 \
        ORDER BY sr.s0, sr.s1, sr.s2, sr.s3, sr.s4, sr.s5
end

set is_organizer $gt_equal (%access($(cnum)) $(priv_instructor))

set hlist $sql_query_open ($(qlist))
#"<tr><td colspan=20>h=$(hlist), $(qlist)</td>
set parity 1
set itemsUsed
while     $sql_query_row  ($(hlist))

   #---Hack to prevent duplicates of same item from appearing.
   #   Don't know why yet! 8/19/2013
   if $tablematch($(items_id) $(itemsUsed))
      continue
   end
   set itemsUsed $(itemsUsed) $(items_id)

   set parity $minus (1 $(parity))
   set cn $(cnum)

   set is_new $empty($(seen))
   set rnew   0
   if $not ($(is_new))
      set rnew $max($minus ($(lastresp) $(seen)) 0)
   end

   set ilabel $item_tuplet ($(item0) $(item1) $(item2) $(item3) $(item4) $(item5))
   "<TR class="itemlist_row$(parity)" valign=top>

   #---Some special applications get a checkbox in front of each item.
   if $equal (x$inc(2) x1)
      "<TD ALIGN=center><INPUT TYPE=checkbox NAME=ibox VALUE="$(ilabel)"
      "    %chk($(items_id))></TD>
   end

   set itemlink
   for col in $unquote($inc(4))

      #---Item number.
      if $equal($(col) number)
          if $empty($(qkey))
             set itemlink <a href=%to_parent(imanage.cml?$(nch)+$(nxt)+\
                              $(cn)+$(items_id)+0+0+x+x)>$(ilabel)</A>
          end
          elif $(is_organizer)
             set itemlink <a href=%to_parent(quiz.cml?$(nch)+$(nxt)+\
                              $(cn)+$(items_id)+0+0+$(qkey)+x)>$(ilabel)</A>
          end
          else
             set itemlink $(ilabel)
          end

          "<td class="itemlist_row_num">
          "   $ifthen ($not($(item1)) $(itemlink))</td>
      end
  
      #---Item title.
      if $equal($(col) title)
         set spacer
         "<TD class="itemlist_row_text"><table cellspacing=0 cellpadding=0><tr valign=top>
         if $and ($not_empty ($(itemlink)) $(item1))
            set padding $mult (2 $minus($sizeof($replace(. 32 $(ilabel))) 2))
            "<td><span style="padding-left: $(padding)em;"
            "   >$(itemlink)</span>&nbsp;</td>
            set spacer &nbsp;
         end

         "<td>$(spacer)</td>
         "<td>
         if $empty($(qkey))
#           "<img $(src_balloon)>
            "<A HREF=%to_parent(viewitem.cml?$(nch)+$(nxt)+\
                        $(cn)+$(items_id)+0+0+1+x#here)>
         end
         else
            "<A HREF=%to_parent(quiz_take.cml?$(nch)+$(nxt)+\
                        $(cn)+$(items_id)+x+x+x+x)>
         end

         if $not ($(forgot))
            "   $t2hbr($(title))</A>
         end
         else
            "<I>$t2hbr($(title))</I></A>
         end
         "</td></table>

#        if $empty($(qkey))
#           "<img $(src_balloon)>
#        end
#        else
#           "<img $(src_quiz)>
#        end
         "</TD>
      end
  
      #---Author.
      if $equal ($(col) author)
         set authorid $(userid)
         set authornm $(fname) $(lname) $if ($(showUserid) ($(authorid)))
         if $empty($(uu))
            set authornm ($(userid))
         end
         set nobr0
         set nobr1
         if $less ($strlen ($(authornm)) 40)
            set nobr0 <nobr>
            set nobr1 </nobr>
         end

         #---If list appears in a pop-up window, references to
         #   authors must either replace the pop-up, or (if editing
         #   one's own description) the parent window.
         if 0$inc(6)
            if $equal ($(authorid) $userid())
               "<TD class="itemlist_row_text"
               " >$(nobr0)<A HREF=%to_parent(me.cml?$(nch)+$(nxt)+$arg(3)+\
                 $arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)+$(authorid)) \
                   %help(h_youare) >$(authornm)</A>$(nobr1)</TD>
            end
            else
               "<TD class="itemlist_row_text"
               " >$(nobr0)<A HREF="person.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+\
                   $arg(5)+$arg(6)+$arg(7)+$arg(8)+$(authorid)" \
                   %help(h_youare) >$(authornm)</A>$(nobr1)</TD>
            end
         end
         elif $empty ($(uu))
           "<TD class="itemlist_row_text">($(userid))</td>
         end
         else
           "<TD class="itemlist_row_text"
           "  >$(nobr0)%person($(authorid) \
                               $quote($(fname) $(lname)))$(nobr1)</td>
         end
      end

      set itemType
      if $equal (x$(type) xwiki)
         set itemType <img $(src_wiki) align="left" hspace="2">
      end
      if $or ($equal (x$(type) xfreeblog) $equal (x$(type) xmodblog))
         set itemType <img $(src_blog) align="left" hspace="2">
      end
  
      #---Report on what's new to me.
      if $equal ($(col) new)
         if $not_empty($(qkey))
            if $empty($(seen))
               "<TD class="itemlist_row_text" 
               "   ><A HREF=%to_parent(quiz_take.cml?$(nch)+$(nxt)+\
                            $(cn)+$(items_id)+0+0+$(qkey)+x)
               "   ><IMG $(src_isnew) BORDER=0 align=right></A></TD>
            end
            elif $equal($(seen) 1)
               "<td valign=middle align="right"><img $(src_quizhalf) hspace=2
               "   title="Quiz: In Progress"></td>
            end
            elif $equal($(seen) 2)
               "<td align="right"><img $(src_quizdone) hspace=2 title="Quiz: Scored"></td>
            end
            else
               "<td valign=middle align="right"><img $(src_quiznot) hspace=2
               "  title="Quiz: Not Started"></td>
            end
         end

         #---New items.  Display "new" icon.
         elif $(is_new)
            "<TD class="itemlist_row_text" 
            "   >$(itemType) <A HREF=%to_parent(viewitem.cml?$(nch)+$(nxt)+\
                         $(cn)+$(items_id)+0+0+1+x#here)
            "   ><IMG $(src_isnew) BORDER=0 align=right></A></TD>
         end
  
         #---Items with new responses
         elif $(rnew)
            "<TD ALIGN=right
            ">$(itemType) <A HREF=%to_parent(viewitem.cml?$(nch)+$(nxt)+\
             $(cn)+$(items_id)+$plus($(seen) 1)+x+1+x#here)\
             >$(rnew)</A>&nbsp;</TD>
         end
  
         #---Items with no new responses
         else
            "<td>$(itemType)</td>
         end
      end
  
      #---"Last" response (# of responses)
      if $equal ($(col) last)
         if $empty($(qkey))
            "<TD class="itemlist_row_num"
            "  ><A HREF=%to_parent(viewitem.cml?$(nch)+\
                   $(nxt)+$(cn)+$(items_id)+$(lastresp)+x+1+x#here)
            "  >$(lastresp)</A></TD>
         end
         else
            "<td></td>
         end
      end
  
      #---Date of last response
      if $equal ($(col) last_date)
         "<TD class="itemlist_row_num"><small><nobr
         "  >%monthDayYearHourTZ ($(time))
         "    $(timezonename)</nobr></small></TD>
      end

      #---Frozen?
      if $equal ($(col) frozen)
         "<td class="itemlist_row_text" align=center valign=middle
         "  ><img $(src_noice)
         "$ifthen ($(frozen)  ><img $(src_ice)     hspace=2)
         "$ifthen ($(hidden)  ><IMG $(src_cloud)   hspace=2)

         set hiddenByDate $sql_query_select (\
             SELECT COUNT(*) FROM site_data \
              WHERE inum=$(items_id) AND cnum=$(cnum) AND name IN ('visdate0', 'visdate1'))
         "$ifthen ($(hiddenByDate) ><img $(src_calendar) hspace="2")

         "$ifthen ($(retired) ><IMG $(src_retired) hspace=2)
         "></td>
#        "<td>seen=$(seen) forgot=$(forgot)</td>
      end

   end
end
eval $sql_query_close($(hlist))

if   $equal ($inc(3) 1)
   set h $sql_query_open  (DROP TABLE $(sorter))
   eval  $sql_query_close ($(h))
end

"<INPUT TYPE=HIDDEN NAME="code" VALUE="">

#============================================================

