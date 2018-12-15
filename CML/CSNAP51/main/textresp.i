#
#---TEXTRESP.I   Display a single response, in plain-text form.
#
#   inc(1)  4-digit conf num
#   inc(2)  4-digit item num
#   inc(3)  response num
#   inc(4)  is this the last response in this page?
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/16/01 18:38 use m_cnum, m_inum, m_rnum
#-----------------------------------------------------------

#---(To make sure all "no-argument" calls work correctly...)
eval $re_owner($inc(1) $inc(2) $inc(3))
set  vi_inum $inc(2)
set  m_inum  $inc(2)

if $(show_id)
   set uid ($re_owner())
end

#---Item date & time, and author.
#if $equal ($inc(3) 0)
#   "Item $inc(2) $re_time() $re_author($inc(1) $inc(2) $inc(3)) $(uid)
#end

#---Response date & time, and author.
#else
   "+$inc(2):$inc(3)) $re_time() $re_author($inc(1) $inc(2) $inc(3)) $(uid)
#end
"

#---"Copied" marker.
#set copy $re_copier ($inc(1) $inc(2) $inc(3))
#if $not_empty ($(copy))
#   "<I>Copied here by
#   "<A HREF="../PEOPLE/$(copy).htm"
#   ">$per_name($(copy))$if ($(show_id) &nbsp;($(copy)))</A>
#   set copy $re_copied ($inc(1) $inc(2) $inc(3))
#   "from <B>$upper1 ($replace (_ 32 $word (1 $(copy))))</B>
#   "$word(2 $(copy)):$word(3 $(copy))
#   "</I>
#   "<P>
#end

#---Decide which set of prohibited tags to use, based on whether
#   the author was an organizer.
set prohibited $(tag_list)
#if $equal($re_bits() 16)
#   if $not_equal(x$site_data($inc(1) tag_list_org) x)
#      set prohibited $site_data($inc(1) tag_list_org)
#   end
#end
#else
#   if $not_equal(x$site_data($inc(1) tag_list_user) x)
#      set prohibited $site_data($inc(1) tag_list_user)
#   end
#end


#---CML response text
#   (The "set rtext" lines have been broken into two parts, in case
#    we need to do some intermediate processing.  So far that has
#    not been necessary... so we may switch back to the original,
#    commented-out code.)
if $equal ($re_prop() 3)
#  set rtext $cleanhtml(prohibited $protect($mac_expand($reval($re_text()))) \
#            )<BR CLEAR=ALL>
   set rtext $protect($mac_expand($reval($re_text())))
#  set rtext $cleanhtml(prohibited $(rtext)) <BR CLEAR=ALL>
   set rtext $cleanhtml(prohibited $(rtext)) 
end

#---HTML response text
elif $equal ($re_prop() 2)
#  set rtext $cleanhtml(prohibited $protect($mac_expand($re_text()))) \
#            <BR CLEAR=ALL>
   set rtext $protect($mac_expand($re_text()))
#  set rtext $cleanhtml(prohibited $(rtext)) <BR CLEAR=ALL>
   set rtext $cleanhtml(prohibited $(rtext)) 
end

#---"Literal" response text
elif $equal ($re_prop() 1)
#  set rtext <PRE>$cleanhtml (prohibited $protect($mac_expand($t2html(\
#                 $re_text()))))</PRE>
#  set rtext $protect($mac_expand($t2html($re_text())))
   set rtext $protect($mac_expand($re_text()))
#  set rtext $cleanhtml (prohibited $(rtext))
end

#---Word-wrapped response text
else
#  set rtext $cleanhtml (prohibited $protect($mac_expand($wrap2html(\
#            $re_text()))))
   set rtext $protect($mac_expand($wrap2html($re_text())))
   set rtext $protect($mac_expand($re_text()))
#  set rtext $cleanhtml (prohibited $(rtext))
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
"$(rtext)

#---Put a divider between responses (except after the last response).
#if $not_equal ($inc(3) $(rstop))
#   "--------------------------------------
#end

#--------------------------------------------------------------------
