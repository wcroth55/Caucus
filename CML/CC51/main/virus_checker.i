#
#---virus_checker.i   Does a file contain a virus?
#
#   inc(1)  full pathname of file to check
#
#   inc(2)  name of file as known to user (possibly w leading directories)
#
#   inc(3)  name of output variable.  If inc(1) contains
#           a virus, put the name (not path) of inc(2)
#           into it.  Otherwise, clear it.
#
#   Assumes that there is a command-line virus scanner that can
#   be run from either:
#      Config viruschecker filename   (in swebd.conf)
#   or
#      /usr/local/bin/viruschecker
#
#   Either script must take one argument, the file to be scanned,
#   and display "infected" on stdout if the file is infected.
#
#   If there is no such script, virus scanning WILL NOT BE DONE!
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/12/04 New file.
#:CR 09/24/08 Complete rewrite.
#---------------------------------------------------------

set $inc(3)

#---Determine (and remember) what virus checker script we're using.
if $empty($(myVirusChecker))
   set myVirusChecker none
   set viruschecker $config(viruschecker)
   if $empty($(viruschecker))
      set viruschecker /usr/local/bin/viruschecker
   end
   if $exists($(viruschecker))
      set myVirusChecker $(viruschecker)
   end
end

if $equal ($(myVirusChecker) none)
   return
end

set virus_output $xshell (1 1 1  $(myVirusChecker) $inc(1))
if $greater ($str_index (infected x$(virus_output)) 0)

   #---Found a virus, put user's name of file into $inc(3).
   set $inc(3) $inc(2)
   set slash $str_revdex (/ $inc(2))
   if $greater ($(slash) 0)
      set $inc(3) $str_sub ($plus ($(slash) 1) 10000 $inc(2))
   end

end

#---------------------------------------------------------
