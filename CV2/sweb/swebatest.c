/*===============================================================*/
   {
     int rc0, rc1, rc2;
     char buf[10000];

     fprintf (stderr, "sql_exec=%d\n",
              sql_exec (0, "select cnums, name from confs"));
     while ( (rc0 = sql_fetch(0)) != SQL_NO_DATA) {
        rc1 = sql_getchardata (0, buf, 10000);
        fprintf (stderr, "cnum(%d)=%s, ", rc1, buf);
        rc2 = sql_getchardata (0, buf, 10000);
        fprintf (stderr, "name(%d)=%s\n", rc2, buf);
     }

     fprintf (stderr, "sql_exec=%d\n",
              sql_exec (0, "select event, time from events where userid='c52'"));
     while ( (rc0 = sql_fetch(0)) != SQL_NO_DATA) {
        rc1 = sql_getchardata (0, buf, 10000);
        fprintf (stderr, "event(%d)=%s, ", rc1, buf);
        rc2 = sql_getchardata (0, buf, 10000);
        fprintf (stderr, "time(%d)=%s\n", rc2, buf);
     }
   }
/*===============================================================*/
