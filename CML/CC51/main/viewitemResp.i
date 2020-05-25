#
#--- viewitemResp.i
#
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#: CR 08/32/09 Correct is_ins vs is_incl confusion.
#: CR 03/06/10 Creating breakouts: must not be frozen, must have access > readonly.
#: CR 04/24/10 Show breakout links even if containing response is deleted!
#: CR 01/23/20 Only allow item deletion if ideletor key/value true (can item authors delete ENTIRE item?)
#------------------------------------------------------------------------------------------

   set _gallery
   set m_rnum $(rnum)
   set is_own   $equal ($(owner) $userid())
   set filtered $and ($(filter_bozos) $tablefind ($(owner) $(bozos)))

   #---Is this response RPOS?  If so, make sure browser scrolls to here.
   if $and ($gt_equal ($(rnum) $(rpos))  $greater($(rnum) 0))
      set rpos 10000000
      if $and ($(is_explorer4) $not ($(is_explorer55)))
         "<SPAN ID="here1"></SPAN>
      end
      else
         "<A NAME="here"><IMG SRC="$(icon)/clearpix.gif" width=1 HEIGHT=1></A>
      end
   end

   #---Skip bozo filtered responses altogether.
   if $(filtered)
      continue
   end


   #---Skip deleted responses without breakouts.
   set breakoutText
   if $greater ($(breakouts) 0)
      set breakoutText $includecml($(main)/showBreakoutLink.i $(vi_cnum) $(vi_inum) $(rnum))
   end
   if $and ($(deleted) $less ($strlen($(breakoutText)) 5))
      continue
   end

   #---Build up list of responses we actually viewed.
   if $(reverse)
      set RespSeenList $(rnum) $(RespSeenList)
   end
   else
      set RespSeenList $(RespSeenList) $(rnum)
   end

   #---Build the EDIT button text if this response is editable,
   #   and if it is SAFE (proper permission and within the time
   #   limit) to edit it.
   set edit
   set delete
   set safeToEdit 0
   set isWiki $and ($equal($(type) wiki) $equal($(rnum) 0))
   if $and ($(editable) $(co_change)  $(is_incl) $equal ($(owner) $userid()))

      set elapsed %epochOfyyyy($(time))
      set safeToEdit 1
      set changelimit $site_data($(vi_cnum) - changelimit)

      #--- If there's no time limit, everything's editable
      if $not_empty ($(changelimit))
         set safeToEdit $gt_equal($(changelimit) $minus($time() $(elapsed)) )
      end
   end
  
   set like_org $gt_equal ($(itemAccess) $priv(instructor))

   if $or ($(is_org) $(like_org) $(safeToEdit))
      set safeToEdit 1
      set edit <A HREF="viedit.cml?$(nch)+$(nxt)+\
                $(vi_cnum)+$(vi_inum)+$(rnum)+x+$arg(7)+$arg(8)"\
                %help(h_editr)>Edit</a>
      set delhref idel.cml?$(nch)+$(nxt)+$(vi_cnum)+$(vi_inum)+$(rnum)+\
                           x+$arg(7)+$arg(8)
      if $(use_javascript)
         set delete <a href="#" \
               onClick="return deleter ('OK to delete response $(rnum)?', \
                        '$(delhref)');">Delete</a>
      end
   end

   if $(showUserid)
      set uid &nbsp;($(owner))
   end
   "<div class="response">

   #---Copied marker, if any.
   set copy

   if $and ($not_empty($(copier)) $(copy_show))
      set copy <BR>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>Copied here by \
               %person($(copier)) from \
               %displayed_conf_name ($(copy_cnum)) \
               $item_label($(copy_cnum) $(copy_item)):$(copy_rnum) \
               on %monthDayYearHourTZ($(copy_time)) $(timezonename)</i>
   end

   "<form NAME="scroll$(rnum)">
   "<table cellspacing=0 cellpadding=0 border=0 width="100%">

   include $(main)/setPersonName.i

   #---Item date & time, and author.
   if $equal ($(rnum) 0)
      "<tr><td width="90%">
      "<span $ifthen ($less ($(save_seen) $(rnum)) style="color: red;")>
      "<span class="response_id">Item $(itemLabel)&nbsp;</span>
      "</span>

      "<span class="response_author">$(personNameLink)</span>
      "<span class="response_note">$(personNameNote)</span>

      "<span class="response_date">
      "%monthDayYearHourTZ ($words (1 2 $(copytime) $(time)))
      "   $(timezonename)</span>

      "$ifthen ($(filtered) \
                <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
      "$(copy)
      "&nbsp;
      if $and ($(showGeoLoc)  \
               $not_equal (none $word (1 $(respLatLong) none)) \
               $not_empty ($(respLocation)) )
         "<a href="editRespLatLongOnMap.cml?$unique()+$(vi_inum)+$(rnum)+$(safeToEdit)+\
            $(respLatLong)+$url_encode($quote($(respLocation)))" 
         "   onClick="return openEditMap();"   TARGET="editMap"
         "  >$(respLocation)</a>&nbsp;&nbsp;
      end
      "</td>

      "<td align=right><span class="response_actionbar"><nobr>
          if $greater ($(items_parent) 0)
             if $or ($(is_org) $(like_org))
                include $(main)/viewitemBlogApprover.i
             end
          end

          #---Organizer, or included owner (when owners are allowed item
          #   deletion) can delete entire item.
          if $or ($(is_org) $(like_org) \
                  $and ($(is_own) $(is_incl)  \
                        $equal(x$site_data($(vi_cnum) - ideletor) x1)) )
             set delhref idel.cml?$(nch)+$(nxt)+$(vi_cnum)+$(vi_inum)+0+\
                                  x+$arg(7)+$arg(8)
             if $(use_javascript)
                "<a href="#" onClick="return deleter (\
                  'OK to delete item and *all* responses\nand *all* sub-items?', \
                  '$(delhref)');">Delete</a> |
             end
          end
          if $or ($(is_org) $(like_org) \
                  $and ($(is_own) $(is_incl) $(editable) $(co_change)))
             "<A HREF="imanage.cml?$(nch)+$(nxt)+\
                            $(vi_cnum)+$(vi_inum)+0+0+$arg(7)+$arg(8)"\
                     %help(h_editi)>Edit</A> |
          end
          elif $(isWiki)
             "<A HREF="viedit.cml?$(nch)+$(nxt)+\
                $(vi_cnum)+$(vi_inum)+$(rnum)+x+$arg(7)+$arg(8)"\
                %help(h_editr)>Edit</a> |
          end
      "   <a href="marknew.cml?$(nch)+$(nxt)+$(vi_cnum)+$(vi_inum)+\
                               $(rnum)+x+$arg(7)+$arg(8)">Mark New</a> |
          set breakoutLink Breakout
          set showBreakoutLink $and ($(allow_breakouts) $(use_javascript) \
                                     $greater ($(itemAccess) $priv(readonly)) $not($(frozen)))

          set isBlog $tablefind ($(type) freeblog modblog)
          if $(isBlog)
             set breakoutLink Add&nbsp;Post
             if $greater ($str_index(. $(itemLabel)))
                set showBreakoutLink 0
             end
          end

          if $(showBreakoutLink)
             #---if primary level, blog, set breakout text
             #   if secondary blog, no breakout
             "<a href="#" onClick="pop_up('break', '$(vi_inum)+$(rnum)'); \
                          return false;">$(breakoutLink)</a> |
          end


          if $and ($not (0$site_data (0 - if_$(iface)_hide_acton)) $(use_javascript))
      "      <a href="#"
      "      onClick="document.scroll$(rnum).bx.checked = true; \
                      pop_up('indv', 'A Title'); \
                      return false;">Select</a>
          end

      "</nobr></span></td>
      if $and ($not (0$site_data (0 - if_$(iface)_hide_acton)) $(use_javascript))
         "<td align=right><INPUT TYPE=CHECKBOX NAME="bx" VALUE="$(rnum)"
             "    onClick="if (document.scroll$(rnum).bx.checked) \
                              pop_up('indv', 'A Title');"></td>
      end
   end

   #---Response date & time, and author.
   else
      "<tr><td width="90%">
      "<span class="response_id">Response
         "<span $ifthen ($less ($(save_seen) $(rnum))  \
                         style="color: red;")>
         "$(itemLabel) : $(rnum)</span></span>
  
      "<span class="response_author">$(personNameLink)</span>
      "<span class="response_note">$(personNameNote)</span>
      "<span class="response_date">
      "%monthDayYearHourTZ ($words (1 2 $(copytime) $(time)))
      "   $(timezonename)</span>
      "$ifthen ($(filtered) \
                <span class="bozofiltered">&nbsp;&nbsp;&nbsp;Filtered</span>)
      "$(copy)
      "&nbsp;
      if $and ($(showGeoLoc)  \
               $not_equal (none $word (1 $(respLatLong) none)) \
               $not_empty ($(respLocation)) )
         "<a href="editRespLatLongOnMap.cml?$unique()+$(vi_inum)+$(rnum)+$(safeToEdit)+\
            $(respLatLong)+$url_encode($quote($(respLocation)))" 
         "   onClick="return openEditMap();"   TARGET="editMap"
         "  >$(respLocation)</a>&nbsp;&nbsp;
      end
      "</td>

      "<td align=right><nobr><span class="response_actionbar">
          if $not_empty ($(delete))
      "      $(delete) |
          end
          if $not_empty ($(edit))
      "      $(edit) |
          end
      "   <a href="marknew.cml?$(nch)+$(nxt)+$(vi_cnum)+$(vi_inum)+\
                               $(rnum)+x+$arg(7)+$arg(8)">Mark New</a> |
          if $and ($(allow_breakouts) $(use_javascript) \
                   $greater ($(itemAccess) $priv(readonly)) $not($(frozen)))
             "<a href="#" onClick="pop_up('break', '$(vi_inum)+$(rnum)'); \
                          return false;">Breakout</a> |
          end

          if 0$(directResponseLink)
#            if $or($(like_org) $(is_org))
                "<a href="#"; onClick="addResponseLink($(rnum), '$(fname) $(lname)'); return false;">Respond</a> |
#            end
          end

          if $and ($not (0$site_data (0 - if_$(iface)_hide_acton)) $(use_javascript))
      "      <a href="#"
      "      onClick="document.scroll$(rnum).bx.checked = true; \
                      pop_up('indv', 'A Title'); \
                      return false;">Select</a>
          end
      "</span></nobr></td>
      if $and ($not (0$site_data (0 - if_$(iface)_hide_acton)) $(use_javascript))
         "<td align=right><INPUT TYPE=CHECKBOX NAME="bx" VALUE="$(rnum)"
             "    onClick="if (document.scroll$(rnum).bx.checked) \
                              pop_up('indv', 'A Title');"></td>
      end
   end
   "</table>
   "</form>

   #---Decide which set of prohibited tags to use, based on whether
   #   the author was an organizer.
   include $(main)/getProhibitedTagList.i $(bits)

   "<div class="response_body">
   "<div style="$ifthen ($not_empty ($(lmargin))  margin-left: $(lmargin);) \
                $ifthen ($not_empty ($(rmargin)) margin-right: $(rmargin);)">

   #---If we know we have a linked task(s), fetch the data for all of them.
   if $tablefind ($(rnum) $(task_resps))
      set query SELECT t.tkey, t.title, t.lead, t.assigner, t.tgroup \
                  FROM tasks t, task_links l \
                 WHERE l.cnum = $(vi_cnum)  AND l.inum = $(vi_inum) \
                   AND l.rnum = $(rnum) AND t.deleted = 0 \
                   AND t.tid = l.tid  AND  t.current = 1
      set h2 $sql_query_open ($(query))
      "<span class="resp_tasklink">
      set task &nbsp;Task:&nbsp;
      "<table cellspacing=0 cellpadding=0 border=0 width="100%">
      "<tr><td align=right
      "   ><table cellspacing=0 cellpadding=0 border=0>
          while $sql_query_row ($(h2))
             #---Is this user allowed to see this task?
             set display $or ($equal ($userid() $(lead)) \
                              $equal ($userid() $(assigner)))
             if $not ($(display))
                set display $group_access ($userid() $(tgroup))
             end
             if $(display)
                "<tr class="resp_taskback" valign=top><td>$(task)</td>
                "<td><a href="taskx.cml?$(nch)+$(nxt)+$(arglist)+$(tkey)"
                "   >$(title)</a>&nbsp;</td>
                set task
             end
          end
      "   </table></td>
      "</table>
      "</span>
      eval $sql_query_close($(h2))
   end

   if $not ($(filtered))
      #---Show responder's thumbnail?
      if $(showThumbNails)
         set thumb $user_data ($(owner) thumbNailSize)
         if $not_empty ($(thumb))
            "<div class="thumbNail">
            if $not_equal($userid() $(owner))
               "<a href="javascript: pop_up('person', '$(owner)');"
            end
            else
               "<A HREF=%to_parent(me.cml?\$(nch)+\$(nxt)+\$arg(3)+\
                     \$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1)
            end
            "><img src="$(http)://$host()/~$caucus_id()/LIB/PICTURES/\
                        $(owner)/_thumb.jpg" border=0></a></div>
         end
      end

      include $(main)/showResponseText.i $(prop)
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
      "$(breakoutText)
   end
   "</div>
   "</div>
   "</div>
