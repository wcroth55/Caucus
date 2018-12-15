#
#---ENLIST.I     List item titles and entry points for e-mail notification
#
#   Input:
#     $inc(1)   ilist    name of var containing triplet list of item numbers.
#     $inc(2)   check    1=>include check box for each item
#     $inc(3)   sort     column by which the list should be sorted (sort code):
#                          (0=number, 1=title, 2=author, 3=last)
#
#   Output: (to browser)
#     List of items, sorted appropriately.
#
#   Actions:
#
#   Notes:
#     enlist.i must be enclosed in a <FORM NAME="isort"...>,
#     and a <TABLE>.
#
#     Caller must include setsort.js.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/20/03 Add timezonehrs, timezonename interface variables.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#============================================================

set columns $site_data($arg(3) - columns)

if $empty($(columns))
 set columns number title author new last
end

#---Table column captions.  Show currently selected "sort by"
#   caption in italics.  Each link is actually a form submission
#   that causes the list of items to be displayed sorted by the
#   title of that link.


#---Item Number
"<TR>
"<TD></TD>

for col in $(columns)

if $equal ($(col) number)
 "<TD ALIGN=right><B>
 if $(use_button)
    "<INPUT TYPE=button onClick="return setsort(0);"  VALUE="#" >
 end
 else
    "<A HREF="#" onClick="return setsort(0);" %help(h_sort)>
    "$if($equal($inc(3) 0) <I>)#</I></A>
 end
 "</B> </TD>
end


#---Title
if $equal ($(col) title)
 "<TD><NOBR><B>
 if $(use_button)
    "<INPUT TYPE=button onClick="return setsort(1);"  VALUE="Title">
 end
 else
    "<A HREF="#" onClick="return setsort(1);" %help(h_sort)>
    "  $if($equal($inc(3) 1) <I>)Title</I></B></A>
 end
 "<SMALL>&nbsp;&nbsp;&nbsp;
 "(click on column header to sort item list)&nbsp;&nbsp;</SMALL></NOBR></TD>
 "<TD WIDTH=10></TD>
end

#---Author
if $equal ($(col) author)
 "<TD><B>
 if $(use_button)
    "<INPUT TYPE=button onClick="return setsort(2);"  VALUE="Author">
 end
 else
    "<A HREF="#" onClick="return setsort(2);" %help(h_sort)>
    "  $if($equal($inc(3) 2) <I>)Author</I></A>
 end
 "</B></TD>
 "<TD WIDTH=10></TD>
end

#---New/Seen
if $equal ($(col) new)
 "<TD ALIGN=right><B>$if ($empty($inc(4)) New Seen)</B></TD>
 "<TD WIDTH=10></TD>
end

#---Last (Number of last response)
if $equal ($(col) last)
 "<TD ALIGN=right><B>
 if $(use_button)
    "<INPUT TYPE=button onClick="return setsort(3);"  VALUE="Last">
 end
 else
    "<A HREF="#" onClick="return setsort(3);" %help(h_sort)>
    "  $if($equal($inc(3) 3) <I>)Last</I></A>
 end
 "</B></TD>
 "<TD WIDTH=10></TD>   
end

#---Date of last response
if $equal($(col) last_date)
 "<TD ALIGN=RIGHT><B>
 if $(use_button)
    "<INPUT TYPE=button onClick="return setsort(3);"  VALUE="Last Active">
 end
 else
    "<A HREF="#" onClick="return setsort(3);" %help(h_sort)>
    "  $if($equal($inc(3) 3) <I>)Last Active</I></A>
 end
 "</B></TD>
 "<TD WIDTH=10></TD>   
end

end  # for column

if $inc(3)
   set itemlist $item_sort($if($equal($inc(3) 3) -1 1) $inc(3) $($inc(1)))
end
else
   set itemlist $($inc(1))
end

#---Row for each item in '$inc(1)' list.
for cn inum rn in $(itemlist)
   if $it_exists ($(cn) $(inum) $(rn))
      set is_organizer $gt_equal (%access($(cn)) $(priv_instructor))
      set can_manage   $or ($(is_organizer)  \
                            $equal ($re_owner ($(cn) $(inum) 0) $userid() ))
      set is_new       $or ($it_new($(cn) $(inum))  $it_unseen($(cn) $(inum)) )
      set rnew $plus($minus($it_resps($(cn) $(inum)) $it_newr($(cn) $(inum))) 1)
      if $not ($it_visib ($(cn) $(inum)))
         set is_new 0
         set rnew   0
      end

      "<TR VALIGN=top>

      #---Some special applications get a checkbox in front of each item.
      if $equal (x$inc(2) x1)
         "<TD ALIGN=center><INPUT TYPE=checkbox NAME=ibox VALUE="$(inum)"></TD>
      end
      else
         "<TD></TD>
      end

      for col in $(columns)
      #---Item number.
      if $equal($(col) number)
       "<TD ALIGN=right>
       "<A HREF="imanage.cml?$(nch)+$(nxt)+$(cn)+$(inum)+0+0+x+x"
       ">$(inum)</A>&nbsp;</TD>
      end

      #---Item title.
      if $equal($(col) title)
       "<TD><A HREF="viewitem.cml?$(nch)+$(nxt)+\
                     $(cn)+$(inum)+0+0+1+x#here">
       if $it_visib ($(cn) $(inum))
          "   $t2hbr($re_title($(cn) $(inum) 0))</A>
       end
       else
          "<I>$t2hbr($re_title($(cn) $(inum) 0))</I></A>
       end
       "</TD>
       "<TD></TD>
      end

      #---Author.
      if $equal ($(col) author)
       "<TD>%person($re_owner() $quote($re_author))</TD>
       "<TD></TD>
      end

      #---Report on what's new to me.
      if $equal ($(col) new)
       if $empty ($inc(4))
         #---New items.  Display "new" icon.
         if $(is_new)
            "<TD><A HREF="viewitem.cml?$(nch)+$(nxt)+\
                $(cn)+$(inum)+0+0+1+x#here"><IMG $(src_isnew) BORDER=0></A></TD>
         end

         #---Items with new responses
         elif $(rnew)
            "<TD ALIGN=right><A HREF="viewitem.cml?$(nch)+$(nxt)+\
                       $(cn)+$(inum)+$it_newr()+x+1+x#here">$(rnew)</A></TD>
         end

         #---Items with no new responses
         else
            "<TD></TD>
         end
       end

      #---Or on how much person $inc(4) has seen.
       else
         set howmuch $it_howmuch ($(cn) $(inum) $inc(4))
         "<TD ALIGN=right>
         set resps $it_resps ($(cn) $(inum))
         if   $equal ($(howmuch) -2)
            "<I>forgot</I>
         end
         elif $equal ($(howmuch) -1)
            "<IMG $(src_isnew)>
         end
         elif $equal ($(howmuch) $(resps) )
            "<IMG $(src_check)>
         end
         else
            "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
                      $(cn)+$(inum)+$(howmuch)+x+1+x#here">$(howmuch)</A>
         end
         "</TD>
       end
       "<TD></TD>
      end

      #---"Last" response (# of responses)
      if $equal ($(col) last)
       "<TD ALIGN=right><A HREF="viewitem.cml?$(nch)+$(nxt)+\
                $(cn)+$(inum)+$it_resps()+x+1+x#here">$it_resps()</A></TD>
       "<TD></TD>
      end

      #---Date of last response
      if $equal ($(col) last_date)
       "<TD ALIGN=right><FONT SIZE=-1>\
          %datetz ($re_time($(cn) $(inum) $it_lastresp())) $(timezonename)
       "</FONT></TD>
       "<TD></TD>
      end

     end
   end
end

"<INPUT TYPE=HIDDEN NAME="code" VALUE="">

#============================================================
