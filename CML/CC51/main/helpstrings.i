# HELPSTRINGS.I
#
# Help texts for JavaScript onMouseOver tags.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

set h_search Search for keywords in items and responses
set h_help Open the Help window for instructions on using Caucus.
set h_quit End your Caucus session.
set h_people Look up other Caucus users.
set h_goto Go to any location in Caucus
set h_notebook View and edit your personal notebook
set h_youare Click to modify your personal settings.
set h_customize Change the settings for this conference.
set h_manage Manage aspects of this Caucus site.
set h_name Click to learn more about this person.
set h_location Click here to go to this location.

set h_editr Click to edit the text of this response.
set h_editi Click to modify this item.
set h_create Start a new discussion item.
set h_create0 You are not permitted to create items in this conference.
set h_expa Display the entire item on one page.
set h_expa0 The entire item is already visible.
set h_1st Display the first page of this item.
set h_1st0 You are already at the first page of this item.
set h_prev Display the previous page of responses.
set h_prev0 There are no previous responses to this item.
set h_next Display the next page of responses.
set h_next0 There are no more responses to this item.
set h_last Display the last page of responses.
set h_last0 You are already at the last page of this item.
set h_readlist Click here to read all the items listed below.
set h_resign Completely remove yourself from this conference.
set h_seen Set all items as if you had read them.
set h_new Set all items as if you had never read them.
set h_indi Edit the attributes of each item individually.
set h_sort Click here to sort the list by this column.
set h_here The current page.
set h_iman Click here to manage this item.

set h_pers Click here to modify your personal conference list.
set h_anew Click here to read the new material in this conference.
set h_inew Click here to read the new items in this conference.
set h_rnew Click here to read the new responses in this conference.
set h_cname Click here to visit the home page of this conference.
set h_join Click here to join and enter this conference.
set h_read Click here to read everything that is new in all conferences.
set h_allconfs Click here to see the list of conferences you can join.

set h_category Click here to modify this notebook category.
set h_noteedit Click here to change or delete this entry.
set h_noteentry Click here to go to this location.

eval $mac_define (help onMouseOver="window.status='\$(@1)'; return true;" \
                       onMouseOut="window.status=''; return true;" \
                       \$ifthen(\$equal(@2 false) onClick="return false;"))

