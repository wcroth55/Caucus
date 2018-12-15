#
#---ITEMS.I     Display all items/replies of a conference on the 
#               conference.cml page
#
#   Written by Gabor Por, hacking together other CML pages.
#   Needs to be looked over and document, probably many pieces
#   could simply be removed -- but hey, it works!
#                                       
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/01 21:31 Integrate (somewhat) GP's work.
#: CR 11/29/03 Handle "copied by" information correctly.
#: CR 12/20/03 Add timezonehrs, timezonename interface variables.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/07/04 Apply stylesheet classes.
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#============================================================


#---Table column captions.  Show currently selected "sort by"
#   caption in italics.  Each link is actually a form submission
#   that causes the list of items to be displayed sorted by the
#   title of that link.



if $inc(3)
   set itemlist $item_sort(1 $inc(3) $($inc(1)))
end
else
   set itemlist $($inc(1))
end

#---Row for each item in '$inc(1)' list.
for cn inum rn in $(itemlist)
   if $it_exists ($(cn) $(inum) $(rn))
      set is_organizer $gt_equal (%access($(cn)) $priv(instructor))
      set can_manage   $or ($(is_organizer)  \
                            $equal ($re_owner ($(cn) $(inum) 0) $userid() ))
      set is_new       $or ($it_new($(cn) $(inum))  $it_unseen($(cn) $(inum)) )
      set rnew $plus($minus($it_resps($(cn) $(inum)) $it_newr($(cn) $(inum))) 1)
      if $not ($it_visib ($(cn) $(inum)))
         set is_new 0
         set rnew   0
      end
   



set arg(4) $(inum)

#to show whole item
set cur_pageresps 10000


#---When in doubt, index into item list is always 1 (first item).
if $equal ($arg(7) x)
   set arg(7) 1
end

#---Conf/item/resp number supplied for a single item request, but no
#   itemlist: make up our own itemlist.
if $and ($not_equal ($arg(3) x)  $equal ($arg(8) x) )
   set ilist $plusmod ($(ilist) 1 $(ilistmax))
   set ilist$(ilist)  $arg(3) $arg(4) $arg(5) $arg(2)
   set arg(8) ilist$(ilist)
   set vi_markseen 1
end

set vi_cnum $arg(3)
set vi_inum $arg(4)
set vi_rnum $arg(5)

#---If we got here w/o explicit cnum/inum/rnum's, reload this page
#   with them in place.  (This is essential for bookmarking to work.)
if $equal ($arg(3) x)
   set vi_temp $rest( $arg(7)  $($arg(8)) )
   set vi_cnum $word( 1 $(vi_temp) )
   set vi_inum $word( 2 $(vi_temp) )
   set vi_rnum $word( 3 $(vi_temp) )
   "Location: $(mainurl)/viewitem.cml?$(nch)+$arg(2)+\
              $(vi_cnum)+$(vi_inum)+$(vi_rnum)+x+$arg(7)+$arg(8)#here
   "
   return
end


#==========================================================================
#  SECTION B:  Figure out exactly where we are *in* the item.

#---Figure out where (what response) to position the page.  (That's
#   where the "#here" anchor will go.)
if $equal (x $arg(5))
   set rpos $(vi_rnum)
end
else
   set rpos $arg(5)
end

#---Figure out where to start loading responses... unless specified,
#   "go back" CONTEXT undeleted responses.
set rstart $arg(6)
if $equal (x $arg(6))
   set rfound 0
   set rstart 0
   count rt 1 $(rpos)
      set rtest $minus ($(rpos) $(rt))
      if $re_exists ($(vi_cnum) $(vi_inum) $(rtest))
         set rfound $plus ($(rfound) 1)
         if $gt_equal ($(rfound) $(context))
            set rstart $(rtest)
            break
         end
      end
   end
end


#---Figure out where to stop loading responses.  Use the user's preference
#   for  page size, unless (a) the EXPAND button wash pushed, in which case
#   load the entire item, or (b) response text was supplied through arg(9)
#   and arg(10), in which case make sure we get to the end of the item.
set cur_pageresps $word (1 $(cur_pageresps) $(pageresps))
set rlast $it_resps($(vi_cnum) $(vi_inum))
set rstop $min ($(rlast) $plus($(rstart) $(cur_pageresps) -1) )
if $not_empty ($arg(9))
   set rstop $(rlast)
end
set rfound 0
if $less ($(rstop) $(rlast))
   set rstop $(rlast)
   count rtest $(rstart) $(rlast)
      if $re_exists($(vi_cnum) $(vi_inum) $(rtest))
         set rfound $plus ($(rfound) 1)
         if $gt_equal ($(rfound) $(cur_pageresps))
            set rstop $(rtest)
            break
         end
      end
   end
end


#==========================================================================
#  SECTION C:  Actually display the (current page of the) item.


#---Save the record of what the user *had* seen on this item,
#   in case we need to "put it back that way" later.
set save_seen $minus ($it_newr ($(vi_cnum) $(vi_inum)) 1)
set visible   $it_visib ( $(vi_cnum) $(vi_inum) )

#---...and then mark this item as seen even as we display it...
if $empty($(vi_markseen))
   set vi_markseen 1
end
if $and ($(vi_markseen) $gt_equal ($(rstop) $it_newr($(vi_cnum) $(vi_inum))) )
   if $(visible)
      set ignore $set_it_seen( $(vi_cnum) $(vi_inum) $(rstop))
   end
end


#---Item header, logo, and help button

set cnum     $(vi_cnum)

#---Are there pages before and after this one?
set page_before  $greater   ($(rstart) 0)
set page_after   $not_equal ($(rstop)  $it_resps())

#---Item header info.

  "<span class="item_title">$t2hbr($re_title($(vi_cnum) $(vi_inum) 0))</span>

if $not ($(visible))
   "$(for_tag)
end
  

#---OK!  Let's display the item!  Count across the responses...
"<P>
set uid
set m_cnum $(cnum)
set m_inum $(vi_inum)
set filter_bozos $and ($word (1 $(siteBozoFilter) 1) \
                       $word (1 $site_data ($(vi_cnum) - confBozoFilter) 1))
count response $(rstart) $(rstop)
   set m_rnum   $(response)
   set owner    $re_owner($(vi_cnum) $(vi_inum) $(response))
   set filtered $and ($(filter_bozos) $tablefind ($(owner) $(bozos)))

   #---Is this response RPOS?  If so, put "here" tag here.
   if $and ($equal ($(response) $(rpos))  $greater($(response) 0) )
      "<A NAME="here"></A>
   end

   if $re_exists($(vi_cnum) $(vi_inum) $(response))
     
      if $(showUserid)
         set uid &nbsp;($re_owner())
      end
      "<div class="response">

      #---Copied marker, if any.
      set copy
      set copytime
      set copied $re_copied ($(vi_cnum) $(vi_inum) $(response))
      if $not_empty ($word (2 $(copied)))
         set copier $re_copier ($(vi_cnum) $(vi_inum) $(response))
         set anon $word (6 $(copied))
         if $not (0$(anon))
            set copy <BR>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<I>Copied here by \
                     %person_noclick($(copier)) from \
                     <B>$upper1 ($replace (_ 32 $word (1 $(copied))))</B> \
                     $word(2 $(copied)):$word(3 $(copied))</I>
         end
         set copytime $words (4 2 $(copied))
      end

      #---Item date & time, and author.
      if $equal ($(response) 0)
         "<span class="response_id">Item $(vi_inum)&nbsp;</span>

         "<span class="response_author">
         if $equal (x$re_owner() x$(mail_out))
            "$re_author()
         end
         else
            "$if ($equal ($re_owner() _) Anonymous \
                  %person_noclick($re_owner() $quote($re_author())))
         end
         "</span>

         "<span class="response_date">
         "%datetz ($words (1 2 $(copytime) $re_time())) $(timezonename)</span>
         "$ifthen ($(filtered) \
                   <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
         "$(copy)

      end

      #---Response date & time, and author.
      else

         "<span class="response_id">Response $(vi_inum):$(response)&nbsp;</span>
  
         "<span class="response_author">
         if $equal (x$re_owner() x$(mail_out))
            "$re_author()
         end
         else
            "$if ($equal ($re_owner() _) Anonymous \
                  %person_noclick($re_owner() $quote($re_author())))
         end
         "</span>
         "<span class="response_date">
         "%datetz ($words (1 2 $(copytime) $re_time())) $(timezonename)</span>
         "$ifthen ($(filtered) \
                   <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
         "$(copy)
      end

      include $(main)/setProhibitedTags.i $(vi_cnum) $(vi_inum) $(response)

      "<div class="response_body">
      "<div style="$ifthen ($not_empty ($(lmargin))  margin-left: $(lmargin);) \
                   $ifthen ($not_empty ($(rmargin)) margin-right: $(rmargin);)">
      if $not ($(filtered))

         #---CML response text
         if $equal ($re_prop() 3)
            "$cleanhtml(prohibited \
                  $protect($mac_expand($reval($cleanhtml(_empty $re_text()))) ))
            "<BR CLEAR=ALL>
         end
   
         #---HTML response text
         elif $equal ($re_prop() 2)
            "$cleanhtml(prohibited \
                  $protect($mac_expand($cleanhtml(_empty $re_text()))))
            "<BR CLEAR=ALL>
         end
   
         #---"Literal" response text
         elif $equal ($re_prop() 1)
            "<PRE>
            "$cleanhtml (prohibited $protect($mac_expand($t2html($re_text()))))
            "</PRE>
         end
   
         #---Word-wrapped response text
         else
            "$cleanhtml (prohibited $protect($mac_expand($wrap2html($re_text()))))
         end
      end

      "</div>
      "</div>
      "</div>
   end
end
end
  "<p/>
end

#============================================================
