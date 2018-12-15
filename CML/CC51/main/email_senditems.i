#
#---EMAIL_SENDITEMS.I    E-mail selected items to address list.
#
#   Arguments:
#     inc(1)    Name of variable containing triplet item list of items to send
#     inc(2)    Name of variable containing e-mail address list
#     inc(3)    Mark selected responses as "seen"?
#     cname     Name of conference
#
#:CR 12/13/04 Add outbound attachments.
#:CR 09/23/08 Get title directly from MySQL, using macro
#      caused an extra newline in the subject line.
#:CR  4/10/09 Handle new email address format, e.g. caucus-site:Conf:Item@...
#:CR  6/05/09 Change new email address divider from ":" to ".-".
#----------------------------------------------------------------------

#---For each selected item...
for vi_cnum vi_inum vi_rnum in $($inc(1))
   
   #---Redirect the output for this particular item to a temp file.
   set outfile $(tmp)/caumail.$userid().$(vi_cnum).$(vi_inum)
   eval $output ($(outfile) 0764)

   #---Prepare the e-mail subject header (with no "s, please!)
   set rlast  %item_lastresp ($(vi_inum))
   set range  $(vi_rnum)-$(rlast)
   if $equal ($(vi_rnum) $(rlast))
      set range $(vi_rnum)
   end
   set itemTitle $unquote($sql_query_select(SELECT title FROM items WHERE id=$(vi_inum)))
   set subject ::Caucus: $(cname): $item_label($(vi_cnum) $(vi_inum)):$(range) $(itemTitle)
   set subject $replace (" ' $(subject))

   set boundary1 ------------\
                %digit8($random(100000000))%digit8($random(100000000))\
                %digit8($random(100000000))
   set boundary2 ------------\
                %digit8($random(100000000))%digit8($random(100000000))\
                %digit8($random(100000000))

   "From: caucus-$caucus_id().-$replace(32 _ $(cname)).-$item_label($(vi_cnum) $(vi_inum))
   "Subject: $(subject)
   "Content-Type: multipart/mixed;
   " boundary="$(boundary1)"
   "
   "This is a multi-part message in MIME format.
   "--$(boundary1)
   "Content-Type: multipart/alternative;
   " boundary="$(boundary2)"
   "
   "
   "--$(boundary2)
   "Content-Type: text/plain; charset=us-ascii; format=flowed
   "
   #---Actually display the selected item/response text.
   include $(main)/email_writeitem.i  $(vi_rnum) 0 total_written
   "
   "--$(boundary2)
   "Content-Type: text/html; charset=us-ascii
   "
   "<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
   "<html>
   "<head>
   "  <meta http-equiv="Content-Type" content="text/html;charset=ISO-8859-1">
   "  <style type="text/css">
#  "    BODY, TD { font-family: Arial, Helvetica, sans-serif;  font-size: 80%; }
   "    BODY, TD { font-family: Arial, Helvetica, sans-serif;  }
   "  </style>
   "</head>
   "<body>

   #---Actually display the selected item/response text.
   include $(main)/email_writeitem.i  $(vi_rnum) 1  total_written

   "</body></html>
   "
   "--$(boundary2)--
   "

   #---Find each attached file (size > 0) for this item/set-of-responses...
   count thisr $(vi_rnum) $(rlast)
      set libdir $caucus_path()/public_html/LIB/ITEMS/$(vi_inum)/$(thisr)
   
      for attachment size isdir in $dirlist(2 $(libdir))
         if $or ($(isdir)  $equal($(size) 0))
            continue
         end
   
         #---Figure out its extension, and from that, its mime-type.
         set mtype  application/octet-stream
         set dotpos $str_revdex (. $(attachment))
         if $gt_equal ($(dotpos) 0)
            set mext   $str_sub    ($plus ($(dotpos) 1) 1000 $(attachment))
            set mpos   $tablefind ($(mext) $(mime_exts))
            if $(mpos)
               set mtype $word ($(mpos) $(mimetypes))
            end
         end
         "--$(boundary1)
         "Content-Type: $(mtype);
         " name="$(thisr)-$(attachment)"
         "Content-Transfer-Encoding: base64
         "Content-Disposition: inline;
         " filename="$(thisr)-$(attachment)"
         "
         "$encode64($(libdir)/$(attachment))
      end
   end
   "--$(boundary1)--
   "
   
   #---Close the temp file.
   eval $output()
   
   #---Send the mail to each person in the list.
   if $(total_written)
      for address in $($inc(2))
         eval $silent  ($config(sendmail)  <$(outfile) $(address))
         eval $sleep   (1);
      end
   end
#  eval $xshell  (1 1 0 rm -f $(outfile))
   
   #---Finally, mark this item as completely 'seen'.
   if $inc(3)
      eval $item_set_seen ($(vi_cnum) $(vi_inum) $userid() $(rlast))
   end
end

#---------------------------------------------------------------
