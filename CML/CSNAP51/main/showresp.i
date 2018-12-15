#
#---SHOWRESP.I   Display a single response.
#
#   inc(1)  4-digit conf num
#   inc(2)  4-digit item num
#   inc(3)  response num
#   inc(4)  is this the last response in this page?
#   text
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 03/13/00 23:31 Do not show "anonymously copied" responses as copied.
#: CR 12/16/01 18:38 use m_cnum, m_inum, m_rnum
#: CR 12/04/02 Add -p option, do not build/display personal info.
#: CR 02/04/08 Use -p (do_people) option for response author names.
#------------------------------------------------------------------

set  vi_inum $inc(2)
set  m_inum  $inc(2)

if $(show_id)
   set uid &nbsp;($(userid))
end

"<div class="response">

if $(copy_show)
   if $not_empty ($(copy_rnum))
      set anon
      if $not (0$(anon))
         if ($(do_people))
            set copierName %person($(copier))
         end
         else
            set copierName %per_name($(copier))
         end
         set copy &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<I>Copied here by \
               $(copierName) from \
               %displayed_conf_name ($(copy_cnum)) \
               $item_label ($(copy_cnum) $(copy_item)):$(copy_rnum) \
               on %monthDayYearHourTZ($(copy_time)) $(timezonename)</I>
      end
   end
end

if $empty($(fname)$(lname))
   set personName ($(userid))
end
elif ($equal ($(userid) _)
   set personName Anonymous
end
elif ($(do_people))
   set personName %person($(userid))
end
else
   set personName %per_name($(userid))
end

#---Item date & time, and author.
if $equal ($inc(3) 0)
   "<span class="response_id"><a name="$inc(3)">Item</a>
   "  $item_label($inc(1) $inc(2))&nbsp;</span>

   "<span class="response_author">
   if $equal (x$(userid) x$(mail_out))
      "$t2mail ($(userid))
   end
   else
      "$(personName)
   end
   "</span>
   "<span class="response_date">
   set timestr %monthDayYearHourTZ ($(time)) $(timezonename)
   if $inc(4)
      "<a name="last">$(timestr)</a>
   end
   else
      "$(timestr)
   end
   "</span>
   "<br>
   "$(copy)
end

#---Response date & time, and author.
else
   "<div style="margin-top: 0.5em;">
   "<span class="response_id"><a name="$inc(3)"
   "   >Response $item_label($inc(1) $(vi_inum)):$inc(3)</a></span>

   "<span class="response_author">
   if $equal (x$(userid) x$(mail_out))
      "$t2mail ($(fname) $(lname))
   end
   else
      "$(personName)
   end
   "</span>

   "<span class="response_date">
   set timestr %monthDayYearHourTZ ($words (1 2 $(copytime) $(time))) $(timezonename)
   if $inc(4)
      "<a NAME="last">$(timestr)</a>
   end
   else
      "$(timestr)
   end
   "</span>
   "</div>
   "$(copy)
end

#---Decide which set of prohibited tags to use, based on whether
#   the author was an organizer.
set prohibited $(tag_list)
if $equal($re_bits() 16)
   if $not_equal(x$site_data($inc(1) - tag_list_org) x)
      set prohibited $site_data($inc(1) - tag_list_org)
   end
end
else
   if $not_equal(x$site_data($inc(1) - tag_list_user) x)
      set prohibited $site_data($inc(1) - tag_list_user)
   end
end

"<div class="response_body">
"<div style="$ifthen ($not_empty ($(lmargin))  margin-left: $(lmargin);) \
             $ifthen ($not_empty ($(rmargin)) margin-right: $(rmargin);)">

#---CML response text
#   (The "set rtext" lines have been broken into two parts, in case
#    we need to do some intermediate processing.  So far that has
#    not been necessary... so we may switch back to the original,
#    commented-out code.)
if   $equal ($(prop) 3)
   set rtext $protect($mac_expand($reval($(text))))
   set rtext $cleanhtml(prohibited $(rtext)) <BR CLEAR=ALL>
end

#---HTML response text
elif $equal ($(prop) 2)
   set rtext $protect($mac_expand($(text)))
   set rtext $cleanhtml(prohibited $(rtext)) <BR CLEAR=ALL>
end

#---"Literal" response text
elif $equal ($(prop) 1)
   set rtext $protect($mac_expand($t2html($(text))))
   set rtext <PRE>$cleanhtml (prohibited $(rtext))</PRE>
end

#---Word-wrapped response text
else
   set rtext $protect($mac_expand($wrap2html($(text))))
   set rtext $cleanhtml (prohibited $(rtext))
end

#---Convert all "old-style" (pre-macro) uploaded file references to
#   a reference to the copied-over library.
set liburl http://$host()/~$caucus_id()/LIB/
while 1
   set lpos $str_index ($(liburl) $(rtext))
   if $less ($(lpos) 0)
      break
   end
   set part1 $str_sub (0 $(lpos) $(rtext))
   set rpos  $plus ($(lpos) $strlen($(liburl)))
   set part2 $str_sub ($(rpos) 1000000 $(rtext))
   set rtext $(part1)LIB/$(part2)
end

#---Convert any truly ancient references to GIF40 (such as some in the
#   demonstration conference) to the copied-over GIF40 directory.
set badgif /~$caucus_id()/GIF40/
while 1
   set lpos $str_index ($(badgif) $(rtext))
   if $less ($(lpos) 0)
      break
   end
   set part1 $str_sub (0 $(lpos) $(rtext))
   set rpos  $plus ($(lpos) $strlen($(badgif)))
   set part2 $str_sub ($(rpos) 1000000 $(rtext))
   set rtext $(part1)GIF50/$(part2)
end
"<table cellspacing=0 cellpadding=0 border=0 width="100%">
"<tr>
"<td>$(rtext)</td>
"</tr>
"</table>

"</div>
"</div>
"</div>

#--------------------------------------------------------------------
