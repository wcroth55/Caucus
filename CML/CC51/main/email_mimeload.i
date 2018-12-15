#
#---email_mimeload.i   Load mime-types file into two matching variables,
#                mimetypes and mime_exts.
#
#: CR  8/20/96 17:14 First version.
#: CR  8/30/07 Ready for 5.1
#--------------------------------------------------------------------

   set mimetypes
   set mime_exts
   set mimefile $word (1 $config(mimetypes) /etc/mime.types)
   set in $open ($(mimefile) r)
   while $readln ($(in) line)
      if $or ($equal ($str_sub (0 1 $(line)) #) \
              $empty ($word (2 $(line))))
         continue
      end
      set mtype $word (1 $(line))
      for x in $rest (2 $(line))
         set mimetypes $(mimetypes) $(mtype)
         set mime_exts $(mime_exts) $(x)
      end
   end
   eval $close ($(in))

#--------------------------------------------------------------------
