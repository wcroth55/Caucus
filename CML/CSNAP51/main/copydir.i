#
#---COPYDIR.I   Recursive copying of files and directories in a directory.
#
#   Input:
#     $inc(1)    directory name
#     $inc(2)    source root
#     $inc(3)    target root
#
#   Output:
#
#   Purpose:
#     Recursively copy all files/dirs under $inc(2)/$inc(1) to $inc(3)/$inc(1).
#
#   How it works
#
#   Notes:
#: CR  6/28/99 20:59 CSNAP 1.1, new file.
#==========================================================================

for fname fsize isdir in $dirlist (2 $inc(2)/$inc(1))
   if $(isdir)
      include $(dir)/copydir.i $(fname) $inc(2)/$inc(1) $inc(3)/$inc(1)
   end
   else
      eval $copyfile ($inc(2)/$inc(1)/$(fname) $inc(3)/$inc(1)/$(fname) 755)
   end
end

return
