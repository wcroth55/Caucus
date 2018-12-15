#
#---EMAIL_WRITEITEM.I      Write item text to output
#
#   See senditems.i, which includes this file.
#
#   inc(1)   first response
#   inc(2)   Output as HTML?  (1 vs 0)
#   inc(3)   variable name with count of non-deleted responses.
#
#: CR  4/10/09 Handle new email address format, e.g. caucus-site:Conf:Item@...
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#----------------------------------------------------------------------------

set $inc(3)  0
set eol    $ifthen ($inc(2) <br>)
set bold   $ifthen ($inc(2) <b>)
set unbold $ifthen ($inc(2) </b>)

if $inc(2)
   "$email_start()
end

"$(bold)Caucus: $(cname) Item $item_label($(vi_cnum) $(vi_inum)): \
   %item_title($(vi_inum))$(unbold)$(eol)
"$(eol)

set between_responses $(eol)- - - - - - - - - - -$(eol)
set rstart  $inc(1)

set query SELECT r.rnum, r.text, r.bits, r.userid, r.time, r.prop, \
                 r.author_shows_as, u.fname, u.lname \
            FROM resps r \
            LEFT JOIN user_info u ON (u.userid = r.userid) \
           WHERE r.items_id=$(vi_inum) AND r.rnum >=$(rstart) \
             AND r.userid = u.userid   AND r.deleted=0  AND r.version= \
               (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
           ORDER BY r.rnum
set h $sql_query_open ($(query))
while $sql_query_row  ($(h))

   set $inc(3) $plus ($($inc(3)) 1)

   #---Decide which set of prohibited tags to use (for filtering), based on
   #   whether the author was an organizer.
   set prohibited $(tag_list)
   if $equal($(bits) 16)
      if $not_equal(x$site_data($(vi_cnum) - tag_list_org) x)
         set prohibited $site_data($(vi_cnum) - tag_list_org)
      end
   end
   else
      if $not_equal(x$site_data($(vi_cnum) - tag_list_user) x)
         set prohibited $site_data($(vi_cnum) - tag_list_user)
      end
   end
   set prohibited $(prohibited) style,prohibit,all caucus,prohibit,all

   #---Response author and header
   if $not_empty ($(author_shows_as))
      set author  $(author_shows_as)
   end
   elif $empty($(lname))
      set author ($(userid))
   end
   elif $(showUserid)
      set author $(fname) $(lname) ($(userid))
   end
   else
      set author $(fname) $(lname)
   end
   if $equal ($(rnum) 0)
      "$(bold)Item $item_label($(vi_cnum) $(vi_inum))$(unbold) $(time) $(author)$(eol)
   end
   else
      "$(bold)Response $item_label($(vi_cnum) $(vi_inum)):$(rnum)$(unbold) $(time) $(author)$(eol)
   end

   #---Calculate what precisely should be displayed.
   set text_pre
   set text_post

   #---CML response text
   if $equal ($(prop) 3)
      set text $replace (10 32 $(text))
      set text $cleanhtml(prohibited \
            $protect($mac_expand($reval($cleanhtml(_empty $(text)))) ))
      if $inc(2)
         set text_post <BR CLEAR=ALL>
      end
   end
   
   #---HTML response text
   elif $equal ($(prop) 2)
      set text $replace (10 32 $(text))
      set text $cleanhtml(prohibited \
            $protect($mac_expand($cleanhtml(_empty $(text)))))
      if $inc(2)
         set text_post <BR CLEAR=ALL>
      end
   end
   
   #---"Literal" response text
   elif $equal ($(prop) 1)
      set text_pre  <PRE>
      set text $cleanhtml (prohibited \
                           $protect($mac_expand($t2html($(text)))))
      set text_post </PRE>
   end
   
   #---Word-wrapped response text
   else
      set text $cleanhtml (prohibited \
                           $protect($mac_expand($wrap2html($(text)))))
   end

   #---Display it, depending on HTML vs plaintext.
   if $inc(2)
      "$(text_pre)
      "$(text)
      "$(text_post)
   end
   else
      if $not_equal ($(prop) 1)
         set text $replace (10 32 $(text))
      end
      set text $unhtml (p,br,tr,li $(text))
      set text $str_replace (&nbsp; $quote( ) $(text))
      set text $str_replace (&quot; "         $(text))
      "$(text)
   end

   if $not_equal ($(rnum) $(rstop))
      "$(between_responses)
   end
end
if $inc(2)
   "$email_stop()
end

#----------------------------------------------------------------------------
