#
#---UPLOADCHECK.I   Did file upload OK?
#
#   inc(1)   name of directory where file was placed, relative
#            to the caucus file LIB directory.
#   inc(2)   name of file
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 08/08/03 New file.
#: CR 12/16/04 Just check individual file.  Doesn't deal with
#              0 byte files, though...
#------------------------------------------------------------------

#---Primitive approach for now... check every file in directory inc(1),
#   see if it's the desired file.  If it has file size 0, or if we 
#   can't find it, then set upload_failed... which will be detected
#   later by viewitem.cml.

#for name size in $dirlist (1 $caucus_lib()/$inc(1))
#   if $equal ($(name) $inc(2))
#      if $equal ($(size) 0)
#         set upload_failed 1
#      end
#      return
#   end
#end
#
#set upload_failed 1

if $not ($exists ($caucus_lib()/$inc(1)))
   set upload_failed 1
end
