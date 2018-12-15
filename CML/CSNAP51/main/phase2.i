#
#---PHASE2.I   Phase 2 of CSNAP.  Get TARGET directory for produced files.
#
#   Purpose:
#
#   How it works
#
#   Input: from user
#
#   Output: sets 'target', full path of target directory in which to
#      place static HTML files.
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#==========================================================================

#---Phase 2: get TARGET directory to put snapshots.
"The static html (and other) files produced by CSNAP will be put
"into single directory, called the TARGET.  CSNAP will create this
"directory if it does not already exist.

while 1
   "
   "What is the full pathname of the TARGET?
   set target $readterm()

   if $empty ($(target))
      "Target is empty.  Quit? (y/n)
      set answer $str_sub (0 1 $lower ($readterm()))
      if $equal ($(answer) y)
         quit
      end
      "Please try again.
   end

   else
      eval $xshell (1 1 1 mkdir $(target)     2>/dev/null)
      eval $xshell (1 1 1 mkdir $(target)/LIB 2>/dev/null)
      set test $open ($(target)/index.htm w)
      if $(test)
         eval $close ($(test))
         "
         break
      end
      else
         "Cannot write to '$(target)'.  Please try again.
      end
   end
end
