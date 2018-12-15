/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PARSE_ATTACH_MAP.  Parse the file which contains definitions and
/    mappings for File Attachments.
/
/    ok = parse_attach_map (filename);
/
/    Parameters:
/       int   ok;        (result code)
/       Chix  filename;  (file to analyze)
/
/    Purpose:
/       When Caucus(Tool) starts, it must create the customized file
/       attachment definitions and I/O mappings database in memory.
/       It does this by parsing a file which contains the ASCII versions
/       of these mappings.
/
/    How it works:
/       basic text parser, looking for two types of fields: definitions
/       and mappings.
/
/       xxx_no_magic: only one asc/bin definition can have "no magic number".
/       xxx_no_magic=0: there is no def that has "no magic number"
/       xxx_no_magic=1: there is 1  def that has "no magic number"
/
/    Returns: 0 on success
/            -1 if file doesn't exist 
/            -2 on syntax error
/            -3 multiple "no magic number" definitions
/
/    Error Conditions: 
/
/    Side effects:
/
/    Related Functions: analyze_file()
/                       ok_to_display_attach()
/
/    Called by: UI
/
/    Operating System Dependencies:
/
/    Known Bugs:
/
/    Home: attach/parseatt.c
/ */

/*: JV  3/03/93 17:13 Create this. */
/*: CP  7/14/93 16:56 Remove nested comments. */
/*: CP  8/18/93 17:32 Change Attach_Mapping_t.ui_type to Chix. */
/*: CP  8/30/93 16:02 Close & unlock XSAM properly. */

#include <stdio.h>
#include "handicap.h"
#include "done.h"
#include "unitcode.h"
#include "attach.h"

extern Attach_Def attach_def;
extern Chix       att_copydir;

FUNCTION parse_attach_map (filename)
Chix filename;
{
   int   success;
   Chix  line, type, format, display, change, new_bl, old_bl, copydir,
         home, value;
   Attach_Def def;

   home    = chxalloc (L(80), THIN, "analyzefile home");
   line    = chxalloc (L(80), THIN, "analyzefile line");
   type    = chxalloc (L(32), THIN, "analyzefile type");
   value   = chxalloc (L(32), THIN, "analyzefile value");
   format  = chxalloc (L(8), THIN, "analyzefile format");
   old_bl  = chxalloc (L(32), THIN, "parsedef old_bl");
   new_bl  = chxalloc (L(16), THIN, "parsedef new_bl");
   display = chxalloc (L(16), THIN, "analyzefile display");
   change  = chxalloc (L(16), THIN, "analyzefile change");
   copydir = chxalloc (L(16), THIN, "analyzefile copydir");

   chxofascii (format,  "format");
   chxofascii (display, "display");
   chxofascii (change,  "change");
   chxofascii (copydir, "copydir");

   /* Accept tabs as white-space. */
   chxspecial (SC_GET, SC_BLANK, old_bl);
   chxcpy     (new_bl, old_bl);
   chxcatval  (new_bl, THIN, L(' '));
   chxcatval  (new_bl, THIN, L('\t'));
   chxspecial (SC_SET, SC_BLANK, new_bl);

   def = attach_def;
   if (NOT unit_lock (XSAM, READ, L(0), L(0), L(0), L(0), filename))  DONE(-1);
   if (NOT unit_view (XSAM))            { unit_unlk (XSAM);   DONE(-1); }

   /*** Assume we succeeded unless SUCCESS gets set otherwise. */
   success = 0;

   /*** Parse the lines in the attach_map file, one at a time. */
   while ( unit_read (XSAM, line, L(0)) ) {
      chxsimplify  (line);
      if (chxvalue (line, L(0)) == L('#'))  continue;
      if (chxlen   (line) == 0)           continue;

      chxtoken  (type, nullchix, 1, line);
      jixreduce (type);
      if (chxeq (type, format)) {           /* Parse the DEFINITION. */
         if ( (success = parse_definition (line, def)) == 0)
            def = def->next;                /* DEF is the last node. */
         else  break;

      } else if (chxeq (type, display)) {
         if ( (success = parse_mapping (line, 0)) != 0)   break;

      } else if (chxeq (type, change)) {
         if ( (success = parse_mapping (line, 1)) != 0)   break;

      } else if (chxeq (type, copydir)) {
         chxtoken (value, nullchix, 2, line);
         if (chxeq (value, CQ("$HOME"))) {
            syshomedir (home);
            chxcpy     (att_copydir, home);
         }
         else                                { success = -2;   break; }
      } else                                 { success = -2;   break; }
   }
   unit_close (XSAM);
   unit_unlk  (XSAM);

done:
   chxspecial (SC_SET, SC_BLANK, old_bl);
   chxfree (home);
   chxfree (line);
   chxfree (type);
   chxfree (value);
   chxfree (new_bl);
   chxfree (old_bl);
   chxfree (format);
   chxfree (display);
   chxfree (change);
   chxfree (copydir);
   return  (success);
}

/*** Parse a file attachment format definition and add it to the table.
/  LINE is the line from the map file.
/  DEF  is a ptr to the last node in the linked-list of definitions.
/ */

FUNCTION parse_definition (line, def)
Chix line;
Attach_Def def;
{
   static char bin_no_magic = 0;
   static char asc_no_magic = 0;
   char   magic_data_c[64];
   int    temp_int, success, clen, i;
   Chix   name, magic, magic_data, data_type, file_ext;
   Attach_Def new_def;

   name      = chxalloc (L(16), THIN, "parsedef name");
   magic     = chxalloc (L(32), THIN, "parsedef magic");
   file_ext  = chxalloc (L(16), THIN, "parsedef file_ext");
   data_type = chxalloc (L(16), THIN, "parsedef data_type");
   magic_data= chxalloc (L(32), THIN, "parsedef magicdata");

   chxtoken (name,  nullchix, 2, line);
   chxtoken (magic, nullchix, 3, line);
   chxtoken (data_type, nullchix, 4, line);
   chxtoken (file_ext,  nullchix, 5, line);
   new_def = make_def_attach();

   /*** Store format name. */
   chxcpy (new_def->format, name);

   /*** Parse data_type field. */
   jixreduce (data_type);
   switch (chxvalue (data_type, L(0))) {
      case L('a'): new_def->data_type = DATA_ASCII;  break;
      case L('b'): new_def->data_type = DATA_BINARY; break;
      default:      DONE(-2);                        break;
   }

   /*** Parse the magic number.   Only only one binary and one ascii
   /    entry that has no magic number. */
   if (chxeq (magic, CQ("-"))) {
      if (new_def->data_type == DATA_ASCII) {
           if (++asc_no_magic > 1)   DONE (-3);
      }
      else if (++bin_no_magic > 1)   DONE (-3);
   } else {
      if (chxvalue (magic, L(0)) != '0' || chxvalue (magic, L(1)) != 'x')
         DONE (-2);
      chxcatsub (magic_data, magic, L(2), ALLCHARS);
      jixreduce (magic_data);
      clen = chxlen (magic_data);
      new_def->magic_size  = clen/2;            /* Check for even number */
      if (new_def->magic_size * 2 != clen) DONE(-2);    /* of characters */
      ascofchx (magic_data_c, magic_data, L(0), ALLCHARS);
      for (i=0; i< new_def->magic_size; i++) {
         sscanf (magic_data_c + 2*i, "%2x", &temp_int);
         new_def->magic[i] = temp_int;        /* Stuff each byte in. */
      }
   }


   /*** Now parse file extension field. */
   if (NOT chxeq (file_ext, CQ("-")))
      ascofchx (new_def->file_ext, file_ext, L(0), L(5));

   success=0;  /* Success */

done:
   chxfree (name);
   chxfree (magic);
   chxfree (file_ext);
   chxfree (data_type);
   chxfree (magic_data);

   if (success != 0) free_def_attach (new_def);
   else              def->next = new_def;
   return (success);
}


FUNCTION parse_mapping (line, which)
Chix line;
int  which;
{
   char found;
   int  success;
   Chix name, command, ui_type;
   Attach_Def def;
   Attach_Mapping map, new_map;

   name    = chxalloc (L(16), THIN, "parsemap name");
   ui_type = chxalloc (L(80), THIN, "parsemap ui_type");
   command = chxalloc (L(80), THIN, "parsemap command");

   chxtoken (name,    nullchix, 2, line);
   chxtoken (ui_type, nullchix, 3, line);
   chxtoken (nullchix, command, 4, line);

   /*** Look for a definition that matches this mapping. */
   for (def=attach_def->next; def != (Attach_Def) NULL; def = def->next)
      if (chxeq (def->format, name)) {
         found=1; break;
      }

   /*** If we found a format for this mapping, add this to the end of
   /    the linked-list of mappings. */
   new_map = make_map_attach();
   if (found) {
      for (map=def->maps; map->next != NULL; map=map->next);
      new_map->map_type = which ? MAP_CHG : MAP_DISP;
      chxcpy    (new_map->ui_type, ui_type);
      jixreduce (new_map->ui_type);
      if (chxlen (command) > 0) 
         chxcpy (new_map->command, command);
      else DONE(-1);

      success=0;  /* Success */
   }
   else DONE (-2);

done:
   chxfree (name);
   chxfree (ui_type);
   chxfree (command);

   if (success != 0) free_map_attach (new_map);
   else              map->next = new_map;
   return (success);
}

