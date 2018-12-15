#
#---CONFFILE.I   Phase 1 of CSNAP.  Read list of confs from file.
#
#   Purpose:
#
#   How it works
#
#   Input: from file in $(conffile)
#
#   Output: sets 'cnums' to list of conference numbers of conferences
#      that will be included in the snapshot.
#
#   Notes:
#: CR 12/26/99 10:53 CSNAP 1.3 (plaintext & arg integration)
#: CR  3/04/04 If ambiguous match, take exact if it's there, complain abt rest
#: CR 12/10/04 Really only complain about skipped ambiguous conferences!
#==========================================================================

#-------Part I: select conferences.

   for conf in $readfile ($(conffile))
      set conf $replace (32 _ $(conf))
   
      set cnum $cl_list ($(conf))
      if $empty ($(cnum))
      end
      elif $not_empty ($word (2 $(cnum)))
         set ambiguous 1
         for cn in $(cnum)
            if $equal ($(conf) $cl_name($(cn)))
               set cnums $(cnums) $(cn)
               set ambiguous 0
               break
            end
         end
         if $(ambiguous)
            "Skipping '$(conf)', match is ambiguous.
         end
      end
      elif $tablefind ($(cnum) $(cnums))
      end
      else
         set cnums $(cnums) $(cnum)
      end
   end
