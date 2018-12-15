/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** ATTACH.H.   Macro Definitions, Structure definitions, and 
/                function declarations for File Attachments. */

/*: JV  2/24/93 12:18 Add File Attachments. */
/*: CP  8/18/93 17:32 Change Attach_Mapping_t.ui_type to Chix. */

#ifndef ATTACH_H
#define ATTACH_H

#include "chixuse.h"

/* Temporary file location. */
#define ATT_TMP   0
#define ATT_HOME  1
#define ATT_USER  2

#define MAGIC_MAX  32      /* Max size for a magic #. */
#define DATA_ASCII  0      /* Type of data */
#define DATA_BINARY 1      /* Used in Attach_Def.data_type */

#define MAP_DISP   0x01    /* Types of mappings, bit mask, used in */
#define MAP_CHG    0x02    /* Attach_Mapping.map_type */

/* Display Types Used in Attach_Mapping.ui_type and sysglobal:
/                        my_ui_type */
#define TERM_VT100        0   /* Plain VT100 */
#define TERM_ASCII_UNIX   1   /* Generic Unix ASCII */
#define TERM_SUN_X_OL     2   /* OPEN LOOK on a Sun */
#define TERM_SUN_X_MTF    3   /* Motif on a Sun */
#define TERM_HP_MTF       4   /* Motif on HP    */
#define TERM_DEC_MTF      5   /* Motif on DECstation */

/* In-memory representation of a mapping from an Attachment file format
/  to a command for display. */
typedef struct Attach_Mapping_t *Attach_Mapping;
struct Attach_Mapping_t {
   short map_type;      /* OR of MAP_DISP, MAP_CHG */
   Chix  ui_type;       /* ASCII_UNIX, X_SUN, X_DEC, etc. */
   Chix  command;       /* eg. "xterm -e vi $1" */
   Attach_Mapping next;
};

/* In-memory representation of a definition of a format type. */
typedef struct Attach_Def_t *Attach_Def;
struct Attach_Def_t {
   Chix format;            /* ASCII, PostScript, etc. */
   int  magic_size;        /* Size of magic #. */
   char magic[MAGIC_MAX];  /* Magic # at beginning of file */
   char data_type;         /* DATA_ASCII or DATA_BINARY */
   char file_ext[6];       /* Default File Extension */
   Attach_Mapping maps;    /* Linked-list of mappings. */
   Attach_Def     next;
};

/* Information about an attachment. */
typedef struct Attachment_t {
   Chix db_name;        /* Name of the file in the Caucus DB */
                        /* During addition: user-specified source file */
   Chix name;           /* Default name when copied out to a file */
   Attach_Def format;   /* Pointer to a definition and its display mappings */
   int4 size;           /* In bytes */
   short rec_len;       /* Record length of file (where needed) */
} *Attachment;

Attachment     make_attach();
Attach_Def     make_def_attach();
Attach_Def     get_attach_def();
Attach_Def     analyze_file();
Attach_Mapping make_map_attach(), ok_to_display_attach();

#endif 
