#
#---sql_init_grouprules.i   Initialize empty MGR group rules.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 08/13/05 New file.
#:CR 09/22/05 Create all -999 "empty" MGR group rows if there are none.
#:CR 01/16/06 Add quizCreate.
#----------------------------------------------------------

set mgrGroups conferenceCreate conferenceDelete conferenceChange \
    conferenceArchive conferenceRestore conferenceAllPrivileges \
    filesafeManage  \
    groupEditNonManager groupEditAll \
    interfaceManage  \
    passwordChangeUsers passwordChangeManagers passwordChangeAnyone \
    quizCreate \
    userCreate userDelete userEdit userReports \
    systemShutdown systemEditCML systemEditSwitches systemBulletins \
    systemAllPrivileges

#if $not ($sql_query_select (SELECT count(*) FROM grouprules  \
#                            WHERE owner='MGR' and name='conferenceCreate'))

if $not ($sql_query_select (SELECT count(*) FROM grouprules  \
                             WHERE owner='MGR' and access = -999))
   for x in $(mgrGroups)
      set grkey $sql_sequence(grouprules)
      set query INSERT INTO grouprules (grkey, owner, name, access) VALUES \
                                       ($(grkey), 'MGR', '$(x)', -999)
      set h $sql_query_open ($(query))
      eval  $sql_query_close($(h))
   end
end
