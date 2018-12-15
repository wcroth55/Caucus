#
#---RLIBCOPY.I    Copy files from one "Response" LIB directory to another
#
# Input:
#   $inc(1)        source conf num
#   $inc(2)        source item num
#   $inc(3)        source resp num
#
#   $inc(4)        target conf num
#   $inc(5)        target item num
#   $inc(6)        target resp num
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/15/00 12:38 Complete rewrite to recursively handle dirs & sub-dirs.
#---------------------------------------------------------------------------

set oldroot $caucus_lib()/ITEMS/$inc(2)/$inc(3)
set newroot $caucus_lib()/ITEMS/$inc(5)/$inc(6)

#---Copy the top level files, and get the list of any top-level directories.
set examine
for fname size isdir in $dirlist (2 $(oldroot))
   if $(isdir)
      set examine $(examine) $(fname)
   end
   else
      eval $copyfile ($(oldroot)/$(fname) $(newroot)/$(fname) 644)
   end
end

#---Recursively (via the stack 'examine') descend all of the directories.
while $not_empty ($(examine))
   set thisdir $word (1 $(examine))
   set examine $rest (2 $(examine))

   for fname size isdir in $dirlist (2 $(oldroot)/$(thisdir))
      if $(isdir)
         set examine $(thisdir)/$(fname) $(examine)
      end
      else
         eval $copyfile ($(oldroot)/$(thisdir)/$(fname) \
                         $(newroot)/$(thisdir)/$(fname) 644)
      end
   end
end
#---------------------------------------------------------------------------
