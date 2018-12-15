#
#---FIXARGS.I      Adjust the argument list on the top of the
#   stack to use the current arguments.
#
#   inc(1)   fragment name
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

set pop    $stack_pop()
set pop1   $word (1 $(pop))
set arg9   $if ($not_empty($arg(9)) +$arg(9))
set frag   $if ($equal ($(pop1) viewitem.cml) $inc(1) )
set pop    $stack_push ($(pop1) \
                $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)$(arg9)$(frag) \
                        $rest (4 $(pop)))
#-----------------------------------------------------------------
