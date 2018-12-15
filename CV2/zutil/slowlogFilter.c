/* slowlogFilter -- filter & translate MySQL slow log queries.
/
/  Usage:
/     slowlog <slow.log n
/
/     where 'n' is the minimum # of seconds -- all other queries tossed
/
/     reformats to one query per line, with elements separated by tabs, e.g.
/	date-time   	# secs	# rows	 query
/     20120624 12:51:57	27	14022	 SELECT MAX(rnum) FROM resps WHERE items_id = 6693 AND rnum > 13971 AND deleted=0 AND version=0;
/
/  
/  Charles Roth
/  6/24/2012
/  */

#include <stdio.h>

#define SKIP     0
#define QUERY    1

main(int argc, char *argv[]) {
   char line[2000];
   char time[400];
   char qtime[1000];
   int  state = SKIP;
   int  queryTime;
   int  pos;

   int minTime = atoi(argv[1]);

   while (fgets (line, 2000, stdin) != NULL) {
      line[strlen(line)-1] = '\0';

      if (strindex(line, "# Time:") == 0) {
         strcpy (time, line+8);
         fgets (line, 2000, stdin);
         fgets (line, 2000, stdin);
         line[strlen(line)-1] = '\0';
         if (strindex(line, "# Query_time") == 0) {
            sscanf (line+13, "%d", &queryTime);
            if (queryTime >= minTime) {
               strcpy (qtime, line);
               state = QUERY;
               pos = strindex(line, "Rows_examined:");
               printf ("\n20%s\t%d\t%s\t", time, queryTime, line+pos+15);
            }
            else                      state = SKIP;
         }
      }

      else if (state == QUERY)  printf (" %s", line);
      
   }
   printf ("\n");

}


strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}
