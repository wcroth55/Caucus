#
#---quiz_probtext.i    Display text of quiz problem.
#
#   Global vars:
#      texttype 
#      text
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/27/06 New file.
#-----------------------------------------------------------------------


#---CML response text
if $equal ($(texttype) cml)
   "$cleanhtml(prohibited \
         $protect($mac_expand($reval($cleanhtml(_empty $(text)))) ))
#  "<BR CLEAR=ALL>
end
  
#---HTML response text
elif $equal ($(texttype) html)
   set output $cleanhtml(prohibited \
         $protect($mac_expand($cleanhtml(_empty $(text)))))
   if $equal (<p> $lower ($str_sub (0 3 $(output))))
      set output $str_sub (3 1000000 $(output))
   end

   set outlen $minus ($strlen($(output)) 4)
#  "$charvals($(output))<br>
#  "====$t2esc($str_sub ($(outlen) 4 $(output)))====<br>
#  "$t2esc($(output))===<br>
   if $equal (</p> $lower ($str_sub ($(outlen) 4 $(output))))
      set output $str_sub (0 $(outlen) $(output))
      "trimmed<br>
   end
#  "$(output)<BR CLEAR=ALL>
   "$(output)
#  abcdef</p>
#  0123456789
end
  
#---"Literal" response text
elif $equal ($(texttype) literal)
   "<PRE>
   "$cleanhtml (prohibited $protect($mac_expand($t2html($(text)))))
   "</PRE>
#  "<BR CLEAR=ALL>
end
  
#---Word-wrapped response text
else
   "$cleanhtml (prohibited \
                $protect($mac_expand($wrap2html($(text)))))
#  "<BR CLEAR=ALL>
end

