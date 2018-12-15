#
#---yyyymmddof.i   Convert common date formats to MySQL YYYY-MM-DD format.
#
#   Implementation of %yyyymmddOf (date) macro.
#
#   Input:  inc(1), inc(2), inc(3)
#         any of "mm/dd/yy", "mm/dd/yyyy", "mm-dd-yy[yy]", 
#                "mon dd[,] yyyy", "yyyy-mm-dd", "dd-MMM-yyyy".
#
#   Output: _yyyymmdd
#         Date as "YYYY-MM-DD"
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 08/08/2004 Add parsing of dd-MMM-yyyy (e.g. 08-AUG-2004) syntax.
#----------------------------------------------------------------------------

set _yyyymmdd
set _input $inc(1)
set  inc2  $inc(2)
set  inc3  $inc(3)
if $empty ($inc(1))
   return
end

#---Translate "mm-dd-yy[yy]" to "mm/dd/yy[yy]".
set dash  $str_index (- $(_input))
set dash2 $str_index (- $str_sub ($plus($(dash) 1) 1000 $(_input)))

#---Translate "dd-MMM-yyyy" to "mon dd, yyyy".
if $equal ($(dash2) 3)
   set inc2   $str_sub (0 $(dash) $(_input))
   set inc3   $str_sub ($plus($(dash) $(dash2) 2) 4 $(_input))
   set _input $str_sub ($plus($(dash) 1) 3 $(_input))
   set dash   $str_index (- $(_input))
end

#---Translate "mm-dd-yy[yy]" to "mm/dd/yy[yy]".
if $tablefind ($(dash) 1 2)
   set _input $replace (- / $(_input))
end

#---mm/dd/yy, mm/dd/yyyy
if $greater ($str_index (/ $(_input)) 0)
   set indate $replace (/ 32 $(_input))
   set mon  $word (1 $(indate))
   set day  $word (2 $(indate))
   set year $word (3 $(indate))
end

#---MySQL format, but clean it up if needed
elif $greater ($str_index (- $(_input)) 0)
   set indate $replace (- 32 $(_input))
   set year $word (1 $(indate))
   set mon  $word (2 $(indate))
   set day  $word (3 $(indate))
end

#--- "mon dd[,] yyyy"
else
   set indate $lower ($replace (, 32 $(_input) $(inc2) $(inc3)))

   set mon $tablefind ($(_input) jan feb mar apr may jun jul aug sep oct nov dec)
   if $equal ($(mon) 0)
      return
   end
   set day  $word (1 $replace(, 32 $(inc2)))
   set year $(inc3)
end

set mon  $str_sub ($minus ($strlen($(mon)) 1)  2  0$(mon) )
set day  $str_sub ($minus ($strlen($(day)) 1)  2  0$(day) )
set year $if ($greater (100 $(year)) $plus(2000 $(year)) $(year))
set _yyyymmdd $(year)-$(mon)-$(day)
"$(_yyyymmdd)
#----------------------------------------------------------------------------
