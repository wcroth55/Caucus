#
#---viewitema.i   "Print" a specific item.
#
#   inc(1)  conference number
#   inc(2)  item id
#
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#==========================================================================
#  Figure out exactly where we are *in* the item.

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

   set q SELECT r.rnum FROM resps r WHERE r.items_id=$inc(2)  \
            AND r.rnum < $(rpos)  AND  r.deleted=0 AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
          ORDER BY r.rnum DESC LIMIT $(context)
   set contextResponses 0 $sql_query_select($(q))
   set rstart $word ($sizeof($(contextResponses)) $(contextResponses))
end


#---Figure out where to stop loading responses.  Use the user's preference
#   for  page size, unless (a) the EXPAND button wash pushed, in which case
#   load the entire item, or (b) response text was supplied through arg(9)
#   and arg(10), in which case make sure we get to the end of the item.
set cur_pageresps $word (1 $(cur_pageresps) $(pageresps))
set rlast $(lastresp)
set rstop $min ($(rlast) $plus($(rstart) $(cur_pageresps) -1) )
if $not_empty ($arg(9))
   set rstop $(rlast)
end
set rfound 0
if $less ($(rstop) $(rlast))
   set q SELECT r.rnum FROM resps r WHERE r.items_id=$inc(2)  \
            AND r.rnum > $(rpos)  AND  r.deleted=0  AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
          ORDER BY r.rnum LIMIT $(cur_pageresps)
   set responses $sql_query_select($(q))
   set rlast $word ($sizeof($(responses)) $(responses))
end

#---Prepare the list of selected responses to print (empty means print all).
#   Make sure that rstop >= the last response selected.
set checklist $replace (, 32 $arg(10))
if $not_empty ($(checklist))
   set lastresp $word ($sizeof($(checklist)) $(checklist))
   set rstop $max (0$(rstop) $(lastresp))
end



set cnum     $inc(1)

#---Hmm, printing a forgotten item, perhaps?  In which case this 
#   should not be 1.
set visible  1


#---Item header info.
set itemlabel  $item_label($inc(1) $inc(2))

"<table cellpadding=0 cellspacing=0 border=0 width="100%">
"<tr><td>
"<span class="item_title">$(cname) Item $(itemlabel) &nbsp;
"$t2hbr($(title))</span>
"</td>

set q SELECT c.hidden, c.items_parent, c.resps_parent, i.qkey \
        FROM conf_has_items c \
        LEFT JOIN items i \
          ON (i.id = c.items_parent) \
       WHERE c.cnum=$inc(1) AND c.items_id=$inc(2)
set h $sql_query_open ($(q))
eval  $sql_query_row  ($(h))
eval  $sql_query_close($(h))

if $greater ($(items_parent) 0)
   "<td>&nbsp;&nbsp;</td>
   "<td align="right">
   set parentItemLabel $item_label ($inc(1) $(items_parent))
   "<table border="0" cellspacing="1" cellpadding="4" 
   "   bgcolor="#C10000"
   "   style="margin-top: 5px;">
   "<tr><td bgcolor="#FFD0D0"
   "><font face="Arial,Helvetica,sans-serif" 
   " style="font-family: Arial, Helvetica, sans-serif; font-size: 8pt;"
   ">Parent: see
   "$(parentItemLabel) : $(resps_parent) %item_title($(items_parent))
   "</font></td>
   "</table>
   "</td>
end
"</table>

if $not ($(visible))
   "$(for_tag)
end
"<P>


#---OK!  Let's display the item!  Count across the responses...
#"<P>
set uid
set m_cnum $(cnum)
set m_inum $inc(2)
set reverse 0$site_data ($arg(3) $arg(4) reverse)
set filter_bozos $and ($word (1 $(siteBozoFilter) 1) \
                       $word (1 $site_data ($inc(1) - confBozoFilter) 1))
set showThumbNails $and ( \
      $word (1 $site_data ($arg(3) - showThumbNails) $(cc_thumbnail)) \
      $word (1 $user_data ($userid() showThumbNails) 1) )

#---Build query to actually *get* the responses.
set rChecked
if $not_empty ($(checklist))
   set rChecked AND r.rnum IN ($commaList(n $(checklist)))
end
set rquery SELECT r.rnum, r.userid owner, r.text, r.time, r.prop, r.bits, \
                  r.copy_rnum, r.copy_item, r.copier, r.copy_time, \
                  r.copy_show, r.copy_cnum, u.fname, u.lname, \
                  (SELECT count(*) FROM conf_has_items h \
                    WHERE h.items_parent=$inc(2) AND h.cnum=$inc(1) \
                      AND h.resps_parent=r.rnum) breakouts \
             FROM resps r \
             LEFT JOIN user_info u ON (r.userid = u.userid) \
            WHERE r.items_id=$inc(2) \
              AND r.rnum >= $(rstart)  AND r.rnum <= $(rstop) AND r.version= \
                 (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
              $(rChecked) \
              AND r.deleted=0 \
            ORDER BY rnum $ifthen ($(reverse) DESC)
set rh $sql_query_open($(rquery))
while  $sql_query_row ($(rh))
   set _gallery
   set m_rnum   $(rnum)
   set response $(rnum)
   set is_own   $equal ($(owner) $userid())
   set filtered $and ($(filter_bozos) $tablefind ($(owner) $(bozos)))
   
   if $and ($not_empty ($(checklist)) \
            $not ($tablefind ($(rnum) $(checklist))))
      continue
   end

   #---Is this response RPOS?  If so, put "here" tag here.
   if $and ($equal ($(response) $(rpos))  $greater($(response) 0) )
      "<A NAME="here"></A>
   end

   if $(showUserid)
      set uid &nbsp;($(owner))
   end

   "<div class="response">

   if $and ($not_empty($(copier)) $(copy_show))
      set copy <BR>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>Copied here by \
               %person($(copier)) from \
               %displayed_conf_name ($(copy_cnum)) \
               $item_label($(copy_cnum) $(copy_item)):$(copy_rnum) \
               on %monthDayYearHourTZ($(copy_time)) $(timezonename)</i>
   end

   if   $equal (x$(owner) x$(mail_out))
      set personNameLink  $t2mail ($(owner))
   end
   elif $equal ($(owner) _)
      set personNameLink Anonymous
   end
   elif $empty ($(fname)$(lname))
      set personNameLink ($(owner))
   end
   else
      set personNameLink  $(fname) $(lname)  \
          $ifthen(0$(showUserid) <span class="response_uid">($(owner))</span>)
   end

   #---Item date & time, and author.
   if $equal ($(response) 0)
      "<span $ifthen ($less ($(save_seen) $(rnum)) style="color: red;")>
      "<span class="response_id">Item $(itemlabel)&nbsp;</span>
      "</span>

      "<span class="response_author">$(personNameLink)</span>

      "<span class="response_date">
      "%monthDayYearHourTZ ($words (1 2 $(copytime) $(time))) 
      "   $(timezonename)</span>

      "$ifthen ($(filtered) \
                <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
      "$(copy)
   end

   #---Response date & time, and author.
   else
      "<span class="response_id">Response
         "<span $ifthen ($less ($(save_seen) $(rnum))  \
                         style="color: red;")>
         "$(itemlabel) : $(rnum)</span></span>
  
      "<span class="response_author">$(personNameLink)</span>
      "<span class="response_date">
      "%monthDayYearHourTZ ($words (1 2 $(copytime) $(time))) 
      "   $(timezonename)</span>
      "$ifthen ($(filtered) \
                <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
      "$(copy)</td>

   end

   #---Decide which set of prohibited tags to use, based on whether
   #   the author was an organizer.

   set prohibited $(tag_list)
   if $equal($re_bits() 16)
      if $not_equal   (x$site_data($inc(1) - tag_list_org) x)
         set prohibited $site_data($inc(1) - tag_list_org)
      end
   end
   else
      if $not_equal   (x$site_data($inc(1) - tag_list_user) x)
         set prohibited $site_data($inc(1) - tag_list_user)
      end
   end

   "<div class="response_body">
   "<div style="$ifthen ($not_empty ($(lmargin))  margin-left: $(lmargin);) \
                $ifthen ($not_empty ($(rmargin)) margin-right: $(rmargin);)">
   if $not ($(filtered))

      #---Show responder's thumbnail?
      if $(showThumbNails)
         set thumb $user_data ($re_owner() thumbNailSize)
         if $not_empty ($(thumb))
            "<div class="thumbNail"
            "><img src="$(http)://$host()/~$caucus_id()/LIB/PICTURES/\
                        $(owner)/_thumb.jpg"></div>
         end
      end

      #---CML response text
      if $equal ($(prop) 3)
         "$cleanhtml(prohibited \
               $protect($mac_expand($reval($cleanhtml(_empty $(text)))) ))
         "<BR CLEAR=ALL>
      end

      #---HTML response text
      elif $equal ($(prop) 2)
         "$cleanhtml(prohibited \
               $protect($mac_expand($cleanhtml(_empty $(text)))))
         "<BR CLEAR=ALL>
      end

      #---"Literal" response text
      elif $equal ($(prop) 1)
         "<PRE>
         "$cleanhtml (prohibited $protect($mac_expand($t2html($(text)))))
         "</PRE>
         "<BR CLEAR=ALL>
      end

      #---Word-wrapped response text
      else
         "$cleanhtml (prohibited $protect(\
                      $mac_expand($wrap2html($(text)))))
         "<BR CLEAR=ALL>
      end
   end


   #---If this response has a gallery, show it at the bottom of
   #   the response.
   set gheader  <table cellspacing=0 cellpadding=0 border=0  \
                 class="gallerytable">
   set gtrailer
   set gcounter -1
   for guid gresp gname gmedium in $(_gallery)
      "$(gheader)
      set gheader
      set gtrailer </table>
      set gcounter $plusmod($(gcounter) 1 5)
      if $equal ($(gcounter) 0)
         "<tr>
      end
      if $(gmedium)
         "<td align=center><a 
         "   href="/~$caucus_id()/mediumimage.html?\
                      %liburl2($(guid) $(gresp) $(gname))" 
      end
      else
         "<td align=center><a href="%liburl2($(guid) $(gresp) $(gname))" 
      end
      "  %_new_target_window(400 400))
      "  >%libimg2($(guid) $(gresp) $(gname).thumb.jpg hspace=1 vspace=1)\
         </a></td>
   end
   "$(gtrailer)

   if $greater ($(breakouts) 0)
      set bquery \
          SELECT i.title breakTitle, i.id FROM items i, conf_has_items h \
           WHERE i.id = h.items_id \
             AND h.cnum = $inc(1) AND h.items_parent=$inc(2) \
                                  AND h.resps_parent=$(rnum) \
           ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
      set hb $sql_query_open ($(bquery))
      while  $sql_query_row  ($(hb))
         set breakLabel $item_label ($inc(1) $(id))
#        "<table width="220" border="0" cellspacing="1" cellpadding="4" 
         "<table border="0" cellspacing="1" cellpadding="4" 
         "   bgcolor="#006B91"
         "   style="margin-top: 5px;">
         "<tr><td bgcolor="#CEEAF5"
         "><font face="Arial,Helvetica,sans-serif" 
         " style="font-family: Arial, Helvetica, sans-serif; font-size: 8pt;"
         ">Breakout: see
         "$(breakLabel) $(breakTitle)
         "</font></td>
         "</table>
      end
      eval $sql_query_close($(hb))
   end

#     #---Put a divider between responses (except after the last response).
#     if $not_equal ($(response) $(rstop))
#        "$(between_responses)
#     end
   "</div>
   "</div>
   "</div>
end
eval $sql_query_close($(rh))

"&nbsp;<br>
