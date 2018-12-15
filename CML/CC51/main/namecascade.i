#
#---namecascade.i   Fetch values of name cascade for each type
#
#   The cascade values are used to divide up large sets of names
#   (conference names, user first names, user last names, user userids)
#   into smaller, manageable sets.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 05/22/05 New file.
#:CR 11/23/05 CONF uses 'name' field.
#----------------------------------------------------------

set nameCascadeCONF $sql_query_select( \
    SELECT name FROM name_cascade WHERE type='name'   ORDER BY name)

set nameCascadefname $sql_query_select( \
    SELECT name FROM name_cascade WHERE type='fname'  ORDER BY name)

set nameCascadelname $sql_query_select( \
    SELECT name FROM name_cascade WHERE type='lname'  ORDER BY name)

set nameCascadeuserid $sql_query_select( \
    SELECT name FROM name_cascade WHERE type='userid' ORDER BY name)
