#
#---archiveName.i   Figure file-system-friendly name of conference archive file.
#
#   inc(1)   name of output variable
#   inc(2)   $quote()'d conference name.
#
#:CR 03/17/07 New file.
#------------------------------------------------------------------------

set _archname $lower($replace(32 _ $inc(2)))
for gc in ! @ % ^ ~ { } : " ' < > / ?
   set _archname $replace($(gc) _ $(_archname))
end
set $inc(1) $(_archname)
