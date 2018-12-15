#
#---reducedChars.i   Implementation of %reducedChars()
#
#   Purpose: to "reduce" a string to a common set of printable characters.
#      Translates everything to lower case, removes blanks and non-printable
#      chars.
#
#   inc(1)   quoted input string
#
#:CR 09/19/2007 New file.
#-------------------------------------------------------------------------

set _rc
for c in $charvals($lower($inc(1)))
   if $and ($greater ($(c) 32)  $less ($(c) 128))
      set _rc $(_rc)$char($(c))
   end
end
"$(_rc)
