#--- NOTEBOOK.I      Displays a notebook category
#
#--- parameters:
# 
#     $inc(1)    The category name (with underscores)
#     $inc(2)    1 or 0 switch indicating whether to include [EDIT]
#     $inc(3)    category number
#
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 07/31/05 Notebook entries with monitor on have link to new responses.
#:CR 09/20/05 Check for access to item or item hidden==1, => say "no access".
#:CR 09/23/05 Enforce quotes around FONT SIZE argument.
#:CR 10/01/07 Correct "There are new responses" test.
#:CR 07/21/24 Add hack to correct group_access() bug that returns 3 instead of 30, etc.
#---------------------------------------------------------------------
   "<DL>
   "<DT><A HREF=note_cat.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+\
                                      $inc(3)+x+x %help(h_category)>
   "<B>$replace(_ 32 $inc(1))</b></A><BR>
   set item_num 0
   "<DD>
   "<TABLE CELLSPACING=0 CELLPADDING=0>
   for page argu anchor name watch in $user_data($userid() mark_$inc(1))
      set item_num $plus($(item_num) 1)
      "<TR VALIGN=top><TD>
      if $inc(2)
         "<A HREF="note_edit.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+\
            $inc(3)+$(item_num)+x %help(h_noteedit)">\
            <span class="smaller">[EDIT]</span></A>\
            &nbsp;&nbsp;&nbsp;&nbsp;
      end
      set link <A HREF=$(page)?$(nch)+$(argu)$(anchor) %help(h_noteentry)>
      "</TD>
  
      "<TD>
      if $equal($(page) viewitem.cml)
         set arglist $replace(+ 32 $(argu))
         set cnm $word(2 $(arglist))
         set inm $word(3 $(arglist))
#        set rnm $word(4 $(arglist))

         set lastresp
         set query \
             SELECT i.lastresp, h.hidden, s.seen \
               FROM (items i, conf_has_items h) \
               LEFT JOIN resps_seen s \
                 ON (s.items_id=i.id AND s.userid='$userid()') \
              WHERE i.id=$(inm) AND h.items_id=i.id AND h.cnum=$(cnm)
         set h $sql_query_open ($(query))
         eval  $sql_query_row  ($(h))
         eval  $sql_query_close($(h))

         if $empty($(lastresp))
            "$unquote($replace(182 32 $(name)))
            "<FONT COLOR=RED>(This item has been deleted)</FONT>
         end
         else
            #---This is a HACK!  For some reason, the grouprules table SOMETIMES(!) has a value that is 1/10th
            #   the expected value.  Don't know why.  7/21/2024  See https://caucuscare.com/CMLREF/ref412.html#group_access
            set access $group_access ($userid() CONF $(cnm))
            if $between(1 $(access) 9)
               set access $mult($(access) 10)
            end
            if $or ($less ($(access) $priv(minimum)) $(hidden))
               "$unquote($replace(182 32 $(name)))
               "<FONT COLOR=RED>(This item is not accessible) </FONT>
            end
            else
               "$(link)$unquote($replace(182 32 $(name)))</A>
               if $(watch)
                  if $less (0$(seen) $(lastresp))
                    "<FONT COLOR=RED><NOBR>(There are
                    "<a href="viewitem.cml?$(nch)+$(nxt)+$(cnm)+$(inm)+\
                      0$(seen)+x+x+x">new responses</a>
                    "to this item)</NOBR></FONT>
                  end
                  else
                    "<NOBR>(no new responses)</NOBR>
                  end
               end
            end
         end
      end
      else
        "$(link)$unquote($replace(182 32 $(name)))</A>
      end
      "</TD>
   end
   "</TABLE>
   if $equal(x x$user_data($userid() mark_$inc(1)))
      "<I>There are no entries in this category</I><P>
   end
   "</DL>
#-----------------------------------------------------------------
