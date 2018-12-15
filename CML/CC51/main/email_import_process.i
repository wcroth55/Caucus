#
#---email_import_process.i    Process an email message to be imported.
#
#   Process the file in $inc(1), add it as an item or response
#   along with its attached files.  Attached images are actually
#   displayed in the item/response, other files are just shown
#   as links.
#
#   The sending address must be allowed to post in the conference,
#   via the list in the 'email' or 'senders' conf variables.
#
#   inc(1)   filename
#   inc(2)   sender email address
#   inc(3)   quoted Subject field
#   inc(4)   our email address
#
#: CR  7/07/04 New file.
#: CR  8/17/04 Sign items/responses with (optional) fullname for email addr.
#: CR 12/14/04 Strip any attached files called NONAME to prevent reposting
#              of files in replies.
#: CR  4/06/07 Ready for 5.1, use $extract_email()
#: CR  4/10/09 Handle new email address format, e.g. caucus-site:Conf:Item@...
#---------------------------------------------------------------------------

#---Make sure message has ::Caucus: in the subject field.
set caucuspos $str_index (::caucus: $lower($inc(3)))
if $less ($(caucuspos) 0)
   eval $delfile ($inc(1))
   return
end

#---Get the confname and item number.
set temp  $replace (: 32 $str_sub ($(caucuspos) 2000 $inc(3)))
set cname $lower($word (2 $(temp)))
set inum  $word (3 $(temp))

#---Get the conf number.
set cnum $cl_num($(cname))

if $equal ($(cnum) 0)
   eval $delfile($inc(1))
   return
end

#---Return string part after "__"
eval $mac_define (after__ \
        \$set (dblunder \$str_index (__ @1)) \
        \$ifthen (\$greater (\$(dblunder) 0) \
                 \$str_sub (\$plus (\$(dblunder) 2) 2000 @1)))

#---Run the attachment detacher:
set tempdir $xshell (1 1 1 $caucus_path()/SCRIPTS/detacher.py <$inc(1))
set pos     $str_index (path= $(tempdir))
set tempdir $str_sub ($plus ($(pos) 5) 10000 $(tempdir))
set tempdir $word (1 $replace (10 32 $(tempdir)))
set attachments
set images
set textfile
set htmlfile
if $greater ($str_index (detacher $(tempdir)) 0)
   for fullname size in $dirlist (1 $(tempdir))
      set name %after__($(fullname))

      if $empty ($(name))
         set size 0
      end
      if $less ($(size) 1)
         continue
      end
  
#     "fullname=$(fullname) name=$(name) size=$(size)

      set ext
      set dot $str_revdex (. $(name))
      if $greater ($(dot) 0)
         set ext $str_sub ($plus($(dot) 1) 1000 $(name))
      end

      if   $equal ($(name) NONAME.dat)
      end
      elif $equal ($(name) NONAME.txt)
         set textfile     $(textfile) $(fullname)
      end
      elif $equal ($(name) NONAME.htm)
         set htmlfile     $(htmlfile) $(fullname)
      end
      elif $gt_equal ($str_index (NONAME $(name)) 0)
      end
      elif $tablefind (x$(ext) xgif xjpg xjpeg xpng)
         set images       $(images)   $(fullname)
      end
      else
         set attachments $(attachments) $(fullname)
      end
   end
end

#---Make sure we have data after the detacher has finished.
if $empty ($(images)$(attachments)$(textfile)$(htmlfile))
   eval $delfile ($inc(1))
   return
end

set has_textfile 0
set theText
for tfile in $(textfile)
   set prop 0
   set theText $(theText) $readfile ($(tempdir)/$(tfile))$newline()
   set has_textfile 1
end

set lookfile
if $not_empty ($(htmlfile))
   set theText
end
for hfile in $(htmlfile)
   set prop 2
   set theText $(theText) $readfile ($(tempdir)/$(hfile))$newline()
   set has_textfile 2
   set lookfile $(tempdir)/$(hfile)
end
#"lookfile=$(lookfile)
#"has_textfile=$(has_textfile)

if $equal ($(has_textfile) 2)
   set theText $email_extract ($inc(4) $(theText))
end

#---Grab the list of email addresses we'll accept email from.
set email_parts $site_data ($(cnum) - email)$newline()$site_data ($(cnum) - senders)$newline()###EOF
set email
set parts
count i 1 1000000
   set addrname $lines ($(i) 1 $(email_parts))
   if $equal ($word (1 $(addrname)) ###EOF)
      break
   end
   set email $(email) $lower ( $word (1 $(addrname)))
   set parts $(parts) $quote ( $rest (2 $(addrname)))
end


#---Check BOTH "From" fields for a match.
set sendersEmailAddress $lower($inc(2))
set found $tablefind ($(sendersEmailAddress) $(email))
if $not ($(found))
   set sendersEmailAddress $lower($word(2 $xshell(1 1 1 /bin/grep "^From: " $inc(1))))
   set found $tablefind ($(sendersEmailAddress) $(email))
end

#---If found, add the text and any attached files.
if $(found)
   set success 0
   set author $word($(found) $(parts))
   if $equal ($sizeof($unquote($(author))) 0)
      set author $(sendersEmailAddress)
   end
   set author $unquote($(author))

   #---Add new item
   if $empty($(inum))
      if 0$site_data ($(cnum) - co_add)
#        set inum $add_item ($(cnum) $(prop) $userid() $(sendersEmailAddress) \
#                            $(author)  0  $(theText))
         set itemId $item_add ($(cnum) 0 0 $userid() itemTitle)
         set rnum $resp_add ($(cnum) $(itemId) $userid() $(prop) 0 0 $(theText))
         set success $if ($(inum) 1 0)
      end
   end

   #---Add new response
   else
      set itemId $item_id ($(cnum) $(inum))
      set rnum $resp_add ($(cnum) $(itemId) $userid() $(prop) 0 0 $(theText))
      set success $if ($(rnum) 1 0)
#     "cnum=$(cnum), inum=$(inum), rnum=$(rnum), success=$(success)
   end

   #---On success, set the author override name.
   if $(success)
      include $(main)/getCurrentVersionOfResponse.i $(itemId) $(rnum)
      set query UPDATE resps SET author_shows_as='$escsingle($(author))' \
                 WHERE rnum=$(rnum) AND items_id=$(itemId) AND version=$(currentVersion)
      set hauthor $sql_query_open ($(query))
      eval        $sql_query_close($(hauthor))
   end

   #---If it succeeded, add the attachment(s) if any.
   if $(success)
      set fnum 0
      set libname ITEMS/$(itemId)/$(rnum)
      for iname in $(images)
         set fnum $plus ($(fnum) 1)
         set name %after__ ($(iname))
         eval $copy2lib ($(tempdir)/$(iname) $(libname)/$(fnum)-$(name))
         set theText $(theText)\%emImgSep($(prop))\
                               \%libimg2($userid() 1 $(fnum)-$(name))
      end
      for aname in $(attachments)
         set fnum $plus ($(fnum) 1)
         set name %after__ ($(aname))
         eval $copy2lib ($(tempdir)/$(aname) $(libname)/$(fnum)-$(name))
         set theText $(theText)$newline()%libname2($userid() 1 $(fnum)-$(name) $(fnum)-$(name))
      end
      eval $resp_set ($(itemId) $(rnum) text $(theText))
   end

end

#---If not, send it back to the author and complain.
else
   set noperm $(tmp)/caucus.email-noperm.$(pid)
   eval $output($(noperm) 0664)
   "You are not permitted to participate in the $upper1($(cname))
   "conference from the address:
   "   $inc(2)
   "
   "If you have any questions, please contact the organizer
   "at: $user_info ($co_org($(cnum)) email)
   eval $output()

   eval $xshell(1 1 1 $(mailer) <$(noperm) -s "No permission" $inc(2))
   eval $delfile ($(noperm))
end

#---Remove the temporary directory
eval $deldir  ($(tempdir))
eval $delfile ($inc(1))
"$errors()
#----------------------------------------------------------------------
