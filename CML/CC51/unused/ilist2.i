#
#---ILIST2.I     List item titles and entry points 
#     on stripped conference page
#     copied item.i and stripped it
#     by Gabor Por
#
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
#        frozen     is this item frozen?
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/01 21:31 Integrated GP's work, needs redocumenting!
#: CR 12/20/03 Add timezonehrs, timezonename interface variables.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#============================================================

#---Table column captions.  Show currently selected "sort by"
#   caption in italics.  Each link is actually a form submission
#   that causes the list of items to be displayed sorted by the
#   title of that link.

#---Item Number
"<TR VALIGN=top>
"<TD></TD>

for col in $unquote($inc(4))

   #---Item number (link to imanage.cml)
   if $equal ($(col) number)
    "<TD ALIGN=right><B><I>#</I></B> </TD>
   end
  
  
   #---Title
   if $equal ($(col) title)
    "<TD><B><I>Title</I></B></TD>
    "<TD></TD>
    "<TD WIDTH=10></TD>
   end
  
   #---Author
   if $equal ($(col) author)
    "<TD><B><I>Author</I></B></TD>
    "<TD WIDTH=10></TD>
   end
  
   #---New/Seen
   if $equal ($(col) new)
    "<TD ALIGN=right><B>$if ($empty($inc(5)) New Seen)</B></TD>
    "<TD WIDTH=10></TD>
   end
  
   #---Last (Number of last response)
   if $equal ($(col) last)
    "<TD ALIGN=right><B><I>Last</I></B></TD>
    "<TD WIDTH=10></TD>
   end
  
   #---Date of last response
   if $equal($(col) last_date)
    "<TD ALIGN=RIGHT><B><I>Last Active</I></B></TD>
    "<TD WIDTH=10></TD>
   end

   #---Frozen?
   if $equal($(col) frozen)
    "<TD><B>Frozen&nbsp; </B></TD>
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
      set is_new       $or ($it_new($(cn) $(inum))  $it_unseen($(cn) $(inum)) )
      set rnew $plus($minus($it_resps($(cn) $(inum)) $it_newr($(cn) $(inum))) 1)
      if $not ($it_visib ($(cn) $(inum)))
         set is_new 0
         set rnew   0
      end

      "<TR VALIGN=top>

      #---Some special applications get a checkbox in front of each item.
      if $equal (x$inc(2) x1)
         "<TD ALIGN=center><INPUT TYPE=checkbox NAME=ibox VALUE="$(inum)"
         "    %chk($(inum))></TD>
      end
      else
         "<TD></TD>
      end

      for col in $unquote($inc(4))

         #---Item number.
         if $equal($(col) number)
          "<TD ALIGN=right>$(inum)&nbsp;</TD>
         end
  
         #---Item title.
         if $equal($(col) title)
          "<TD COLSPAN=2>
          if $it_visib ($(cn) $(inum))
             "   $t2hbr($re_title($(cn) $(inum) 0))
          end
          else
             "<I>$t2hbr($re_title($(cn) $(inum) 0))</I>
          end
          "</TD>
          "<TD></TD>
         end
  
         #---Author.
         if $equal ($(col) author)
            set authorid $re_owner($(cn) $(inum) 0)
            set authornm %per_name ($(authorid)) $if ($(show_id) ($(authorid)))

            #---If list appears in a pop-up window, references to
            #   authors must either replace the pop-up, or (if editing
            #   one's own description) the parent window.
            if 0$inc(6)
               if $equal ($(authorid) $userid())
                  "<TD>$re_author()</TD>
               end
               else
                  "<TD>$re_author()</TD>
               end
            end
            else
              "<TD>$re_author()</TD>
            end
          "<TD></TD>
         end
  
         #---Report on what's new to me.
         if $equal ($(col) new)
          if $empty ($inc(5))
            #---New items.  Display "new" icon.
            if $(is_new)
               "<TD><IMG $(src_isnew) BORDER=0></TD>
            end
  
            #---Items with new responses
            elif $(rnew)
               "<TD ALIGN=right>$(rnew)</TD>
            end
  
            #---Items with no new responses
            else
               "<TD></TD>
            end
          end
  
         #---Or on how much person $inc(5) has seen.
          else
            set howmuch $it_howmuch ($(cn) $(inum) $inc(5))
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
               "$(howmuch)
            end
            "</TD>
          end
          "<TD></TD>
         end
  
         #---"Last" response (# of responses)
         if $equal ($(col) last)
          "<TD ALIGN=right>$it_resps()</TD>
          "<TD></TD>
         end
  
         #---Date of last response
         if $equal ($(col) last_date)
          "<TD ALIGN=right><TT><FONT SIZE=-1>\
             %datetz($re_time($(cn) $(inum) $it_lastresp()))
          "  $(timezonename)</TT></FONT></TD>
          "<TD></TD>
         end

         #---Frozen?
         if $equal ($(col) frozen)
          "<TD ALIGN=center>
          "$ifthen ($it_frozen ($(cn) $(inum)) <IMG $(src_ice)>)</TD>
         end
  
      end
   end
end

"<INPUT TYPE=HIDDEN NAME="code" VALUE="">

#============================================================
