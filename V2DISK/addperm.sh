#!/bin/sh
#
#---addperm.sh   Add current user with "readonly" access to all confs,
#   to smooth conversion.
#
#----------------------------------------------------------------------
uid=`whoami`
cd $HOME
for x in C[0-9][0-9][0-9][0-9]; do
   (echo ":readonly";  echo "$uid";  cat $x/userlist) >temp
   cp temp $x/userlist
done
