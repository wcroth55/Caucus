#
#---ERRORS.I    Report any syntax or function-call errors accumulated
#   during course of current page.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/04/00 12:46 Flush $errors() even if not displayed to user.
#------------------------------------------------------------------------

#---Should we report CML errors?
set cmlerr $word (1 $user_data ($userid() cmlerr) $(cmlerr_def))

if $equal ($(cmlerr) no)
   #---No, but flush the error messages (else they keep accumulating!)
   eval $errors()
end

else
   #---Yes, and if there are errors, put them in red wherever this page 
   #   was included (typically at the bottom).
   set errtext $errors()
   if $not_empty ($(errtext))
      "<P>
      "<HR>
      "<FONT COLOR=RED>
      "$t2hbr($(errtext))

      #---Optionally list all variables as well, to help in debugging.
      #"<HR>
      #"$t2hbr($variables())

      "</FONT>
      "<HR>
   
      #---Pop-up alert to say there are errors
      if $equal ($(cmlerr) pop)
         "<script type="text/javascript" language="javascript">
         "alert ("See CML errors reported at bottom of page.");
         "</SCRIPT>
      end
   end
end
