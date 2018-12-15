/* tools.c - tools for slap tools */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-bdb/tools.c,v 1.72.2.7 2005/08/09 21:04:02 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2000-2005 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "portable.h"

#include <stdio.h>
#include <ac/string.h>

#define AVL_INTERNAL
#include "back-bdb.h"

static DBC *cursor = NULL;
static DBT key, data;

typedef struct dn_id {
	ID id;
	struct berval dn;
} dn_id;

#define	HOLE_SIZE	4096
static dn_id hbuf[HOLE_SIZE], *holes = hbuf;
static unsigned nhmax = HOLE_SIZE;
static unsigned nholes;

static Avlnode *index_attrs, index_dummy;

int bdb_tool_entry_open(
	BackendDB *be, int mode )
{
	/* initialize key and data thangs */
	DBTzero( &key );
	DBTzero( &data );
	key.flags = DB_DBT_REALLOC;
	data.flags = DB_DBT_REALLOC;

	return 0;
}

int bdb_tool_entry_close(
	BackendDB *be )
{
	assert( be != NULL );

	if( key.data ) {
		ch_free( key.data );
		key.data = NULL;
	}
	if( data.data ) {
		ch_free( data.data );
		data.data = NULL;
	}

	if( cursor ) {
		cursor->c_close( cursor );
		cursor = NULL;
	}

	if( nholes ) {
		unsigned i;
		fprintf( stderr, "Error, entries missing!\n");
		for (i=0; i<nholes; i++) {
			fprintf(stderr, "  entry %ld: %s\n",
				holes[i].id, holes[i].dn.bv_val);
		}
		return -1;
	}
			
	return 0;
}

static int bdb_reindex_cmp(const void *a, const void *b) { return 0; }

ID bdb_tool_entry_next(
	BackendDB *be )
{
	int rc;
	ID id;
	struct bdb_info *bdb = (struct bdb_info *) be->be_private;

	assert( be != NULL );
	assert( slapMode & SLAP_TOOL_MODE );
	assert( bdb != NULL );
	
	/* Initialization */
	if (cursor == NULL) {
		rc = bdb->bi_id2entry->bdi_db->cursor(
			bdb->bi_id2entry->bdi_db, NULL, &cursor,
			bdb->bi_db_opflags );
		if( rc != 0 ) {
			return NOID;
		}
	}

	rc = cursor->c_get( cursor, &key, &data, DB_NEXT );

	if( rc != 0 ) {
		/* If we're doing linear indexing and there are more attrs to
		 * index, and we're at the end of the database, start over.
		 */
		if ( bdb->bi_attrs == &index_dummy ) {
			if ( index_attrs && rc == DB_NOTFOUND ) {
				/* optional - do a checkpoint here? */
				index_dummy.avl_data = avl_delete(&index_attrs, NULL, bdb_reindex_cmp);
				rc = cursor->c_get( cursor, &key, &data, DB_FIRST );
			}
			if ( rc ) {
				bdb->bi_attrs = NULL;
				return NOID;
			}
		} else {
			return NOID;
		}
	}

	if( data.data == NULL ) {
		return NOID;
	}

	BDB_DISK2ID( key.data, &id );
	return id;
}

ID bdb_tool_dn2id_get(
	Backend *be,
	struct berval *dn
)
{
	Operation op = {0};
	Opheader ohdr = {0};
	EntryInfo *ei = NULL;
	int rc;

	if ( BER_BVISEMPTY(dn) )
		return 0;

	op.o_hdr = &ohdr;
	op.o_bd = be;
	op.o_tmpmemctx = NULL;
	op.o_tmpmfuncs = &ch_mfuncs;

	rc = bdb_cache_find_ndn( &op, NULL, dn, &ei );
	if ( ei ) bdb_cache_entryinfo_unlock( ei );
	if ( rc == DB_NOTFOUND )
		return NOID;
	
	return ei->bei_id;
}

int bdb_tool_id2entry_get(
	Backend *be,
	ID id,
	Entry **e
)
{
	int rc = bdb_id2entry( be, NULL, id, e );

	if ( rc == DB_NOTFOUND && id == 0 ) {
		Entry *dummy = ch_calloc( 1, sizeof(Entry) );
		struct berval gluebv = BER_BVC("glue");
		dummy->e_name.bv_val = ch_strdup( "" );
		dummy->e_nname.bv_val = ch_strdup( "" );
		attr_merge_one( dummy, slap_schema.si_ad_objectClass, &gluebv, NULL );
		attr_merge_one( dummy, slap_schema.si_ad_structuralObjectClass,
			&gluebv, NULL );
		*e = dummy;
		rc = LDAP_SUCCESS;
	}
	return rc;
}

Entry* bdb_tool_entry_get( BackendDB *be, ID id )
{
	int rc;
	Entry *e = NULL;
#ifndef BDB_HIER
	struct berval bv;
#endif

	assert( be != NULL );
	assert( slapMode & SLAP_TOOL_MODE );
	assert( data.data != NULL );

#ifndef BDB_HIER
	DBT2bv( &data, &bv );

#ifdef SLAP_ZONE_ALLOC
	/* FIXME: will add ctx later */
	rc = entry_decode( &bv, &e, NULL );
#else
	rc = entry_decode( &bv, &e );
#endif

	if( rc == LDAP_SUCCESS ) {
		e->e_id = id;
	}
#else
	{
		EntryInfo *ei = NULL;
		Operation op = {0};
		Opheader ohdr = {0};

		op.o_hdr = &ohdr;
		op.o_bd = be;
		op.o_tmpmemctx = NULL;
		op.o_tmpmfuncs = &ch_mfuncs;

		rc = bdb_cache_find_id( &op, NULL, id, &ei, 0, 0, NULL );
		if ( rc == LDAP_SUCCESS )
			e = ei->bei_e;
	}
#endif
	return e;
}

static int bdb_tool_next_id(
	Operation *op,
	DB_TXN *tid,
	Entry *e,
	struct berval *text,
	int hole )
{
	struct berval dn = e->e_name;
	struct berval ndn = e->e_nname;
	struct berval pdn, npdn;
	EntryInfo *ei = NULL, eidummy;
	int rc;

	if (ndn.bv_len == 0) {
		e->e_id = 0;
		return 0;
	}

	rc = bdb_cache_find_ndn( op, tid, &ndn, &ei );
	if ( ei ) bdb_cache_entryinfo_unlock( ei );
	if ( rc == DB_NOTFOUND ) {
		if ( !be_issuffix( op->o_bd, &ndn ) ) {
			ID eid = e->e_id;
			dnParent( &dn, &pdn );
			dnParent( &ndn, &npdn );
			e->e_name = pdn;
			e->e_nname = npdn;
			rc = bdb_tool_next_id( op, tid, e, text, 1 );
			e->e_name = dn;
			e->e_nname = ndn;
			if ( rc ) {
				return rc;
			}
			/* If parent didn't exist, it was created just now
			 * and its ID is now in e->e_id. Make sure the current
			 * entry gets added under the new parent ID.
			 */
			if ( eid != e->e_id ) {
				eidummy.bei_id = e->e_id;
				ei = &eidummy;
			}
		}
		rc = bdb_next_id( op->o_bd, tid, &e->e_id );
		if ( rc ) {
			snprintf( text->bv_val, text->bv_len,
				"next_id failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> bdb_tool_next_id: %s\n", text->bv_val, 0, 0 );
			return rc;
		}
		rc = bdb_dn2id_add( op, tid, ei, e );
		if ( rc ) {
			snprintf( text->bv_val, text->bv_len, 
				"dn2id_add failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> bdb_tool_next_id: %s\n", text->bv_val, 0, 0 );
		} else if ( hole ) {
			if ( nholes == nhmax - 1 ) {
				if ( holes == hbuf ) {
					holes = ch_malloc( nhmax * sizeof(dn_id) * 2 );
					AC_MEMCPY( holes, hbuf, sizeof(hbuf) );
				} else {
					holes = ch_realloc( holes, nhmax * sizeof(dn_id) * 2 );
				}
				nhmax *= 2;
			}
			ber_dupbv( &holes[nholes].dn, &ndn );
			holes[nholes++].id = e->e_id;
		}
	} else if ( !hole ) {
		unsigned i;

		e->e_id = ei->bei_id;

		for ( i=0; i<nholes; i++) {
			if ( holes[i].id == e->e_id ) {
				int j;
				free(holes[i].dn.bv_val);
				for (j=i;j<nholes;j++) holes[j] = holes[j+1];
				holes[j].id = 0;
				nholes--;
				break;
			} else if ( holes[i].id > e->e_id ) {
				break;
			}
		}
	}
	return rc;
}

ID bdb_tool_entry_put(
	BackendDB *be,
	Entry *e,
	struct berval *text )
{
	int rc;
	struct bdb_info *bdb = (struct bdb_info *) be->be_private;
	DB_TXN *tid = NULL;
	Operation op = {0};
	Opheader ohdr = {0};

	assert( be != NULL );
	assert( slapMode & SLAP_TOOL_MODE );

	assert( text != NULL );
	assert( text->bv_val != NULL );
	assert( text->bv_val[0] == '\0' );	/* overconservative? */

	Debug( LDAP_DEBUG_TRACE, "=> " LDAP_XSTRING(bdb_tool_entry_put)
		"( %ld, \"%s\" )\n", (long) e->e_id, e->e_dn, 0 );

	if (! (slapMode & SLAP_TOOL_QUICK)) {
	rc = TXN_BEGIN( bdb->bi_dbenv, NULL, &tid, 
		bdb->bi_db_opflags );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
			"txn_begin failed: %s (%d)",
			db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_put) ": %s\n",
			 text->bv_val, 0, 0 );
		return NOID;
	}
	}

	op.o_hdr = &ohdr;
	op.o_bd = be;
	op.o_tmpmemctx = NULL;
	op.o_tmpmfuncs = &ch_mfuncs;

	/* add dn2id indices */
	rc = bdb_tool_next_id( &op, tid, e, text, 0 );
	if( rc != 0 ) {
		goto done;
	}

	/* id2entry index */
	rc = bdb_id2entry_add( be, tid, e );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
				"id2entry_add failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_put) ": %s\n",
			text->bv_val, 0, 0 );
		goto done;
	}

	if ( !bdb->bi_linear_index )
		rc = bdb_index_entry_add( &op, tid, e );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
				"index_entry_add failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_put) ": %s\n",
			text->bv_val, 0, 0 );
		goto done;
	}

done:
	if( rc == 0 ) {
		if ( !( slapMode & SLAP_TOOL_QUICK )) {
		rc = TXN_COMMIT( tid, 0 );
		if( rc != 0 ) {
			snprintf( text->bv_val, text->bv_len,
					"txn_commit failed: %s (%d)",
					db_strerror(rc), rc );
			Debug( LDAP_DEBUG_ANY,
				"=> " LDAP_XSTRING(bdb_tool_entry_put) ": %s\n",
				text->bv_val, 0, 0 );
			e->e_id = NOID;
		}
		}

	} else {
		if ( !( slapMode & SLAP_TOOL_QUICK )) {
		TXN_ABORT( tid );
		snprintf( text->bv_val, text->bv_len,
			"txn_aborted! %s (%d)",
			db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_put) ": %s\n",
			text->bv_val, 0, 0 );
		}
		e->e_id = NOID;
	}

	return e->e_id;
}

int bdb_tool_entry_reindex(
	BackendDB *be,
	ID id )
{
	struct bdb_info *bi = (struct bdb_info *) be->be_private;
	int rc;
	Entry *e;
	DB_TXN *tid = NULL;
	Operation op = {0};
	Opheader ohdr = {0};

	Debug( LDAP_DEBUG_ARGS,
		"=> " LDAP_XSTRING(bdb_tool_entry_reindex) "( %ld )\n",
		(long) id, 0, 0 );

	/* No indexes configured, nothing to do. Could return an
	 * error here to shortcut things.
	 */
	if (!bi->bi_attrs) {
		return 0;
	}

	/* Get the first attribute to index */
	if (bi->bi_linear_index && !index_attrs && bi->bi_attrs != &index_dummy) {
		index_attrs = bi->bi_attrs;
		bi->bi_attrs = &index_dummy;
		index_dummy.avl_data = avl_delete(&index_attrs, NULL, bdb_reindex_cmp);
	}

	e = bdb_tool_entry_get( be, id );

	if( e == NULL ) {
		Debug( LDAP_DEBUG_ANY,
			LDAP_XSTRING(bdb_tool_entry_reindex)
			": could not locate id=%ld\n",
			(long) id, 0, 0 );
		return -1;
	}

	if (! (slapMode & SLAP_TOOL_QUICK)) {
	rc = TXN_BEGIN( bi->bi_dbenv, NULL, &tid, bi->bi_db_opflags );
	if( rc != 0 ) {
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_reindex) ": "
			"txn_begin failed: %s (%d)\n",
			db_strerror(rc), rc, 0 );
		goto done;
	}
	}
 	
	/*
	 * just (re)add them for now
	 * assume that some other routine (not yet implemented)
	 * will zap index databases
	 *
	 */

	Debug( LDAP_DEBUG_TRACE,
		"=> " LDAP_XSTRING(bdb_tool_entry_reindex) "( %ld, \"%s\" )\n",
		(long) id, e->e_dn, 0 );

	op.o_hdr = &ohdr;
	op.o_bd = be;
	op.o_tmpmemctx = NULL;
	op.o_tmpmfuncs = &ch_mfuncs;

	rc = bdb_index_entry_add( &op, tid, e );

done:
	if( rc == 0 ) {
		if (! (slapMode & SLAP_TOOL_QUICK)) {
		rc = TXN_COMMIT( tid, 0 );
		if( rc != 0 ) {
			Debug( LDAP_DEBUG_ANY,
				"=> " LDAP_XSTRING(bdb_tool_entry_reindex)
				": txn_commit failed: %s (%d)\n",
				db_strerror(rc), rc, 0 );
			e->e_id = NOID;
		}
		}

	} else {
		if (! (slapMode & SLAP_TOOL_QUICK)) {
		TXN_ABORT( tid );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_reindex)
			": txn_aborted! %s (%d)\n",
			db_strerror(rc), rc, 0 );
		}
		e->e_id = NOID;
	}
	bdb_entry_release( &op, e, 0 );

	return rc;
}

ID bdb_tool_entry_modify(
	BackendDB *be,
	Entry *e,
	struct berval *text )
{
	int rc;
	struct bdb_info *bdb = (struct bdb_info *) be->be_private;
	DB_TXN *tid = NULL;
	Operation op = {0};
	Opheader ohdr = {0};

	assert( be != NULL );
	assert( slapMode & SLAP_TOOL_MODE );

	assert( text != NULL );
	assert( text->bv_val != NULL );
	assert( text->bv_val[0] == '\0' );	/* overconservative? */

	assert ( e->e_id != NOID );

	Debug( LDAP_DEBUG_TRACE,
		"=> " LDAP_XSTRING(bdb_tool_entry_modify) "( %ld, \"%s\" )\n",
		(long) e->e_id, e->e_dn, 0 );

	if (! (slapMode & SLAP_TOOL_QUICK)) {
	rc = TXN_BEGIN( bdb->bi_dbenv, NULL, &tid, 
		bdb->bi_db_opflags );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
			"txn_begin failed: %s (%d)",
			db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": %s\n",
			 text->bv_val, 0, 0 );
		return NOID;
	}
	}

	op.o_hdr = &ohdr;
	op.o_bd = be;
	op.o_tmpmemctx = NULL;
	op.o_tmpmfuncs = &ch_mfuncs;

	/* id2entry index */
	rc = bdb_id2entry_update( be, tid, e );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
				"id2entry_add failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": %s\n",
			text->bv_val, 0, 0 );
		goto done;
	}

#if 0
	/* FIXME: this is bogus, we don't have the old values to delete
	 * from the index because the given entry has already been modified.
	 */
	rc = bdb_index_entry_del( &op, tid, e );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
				"index_entry_del failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": %s\n",
			text->bv_val, 0, 0 );
		goto done;
	}
#endif

	rc = bdb_index_entry_add( &op, tid, e );
	if( rc != 0 ) {
		snprintf( text->bv_val, text->bv_len,
				"index_entry_add failed: %s (%d)",
				db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": %s\n",
			text->bv_val, 0, 0 );
		goto done;
	}

done:
	if( rc == 0 ) {
		if (! (slapMode & SLAP_TOOL_QUICK)) {
		rc = TXN_COMMIT( tid, 0 );
		if( rc != 0 ) {
			snprintf( text->bv_val, text->bv_len,
					"txn_commit failed: %s (%d)",
					db_strerror(rc), rc );
			Debug( LDAP_DEBUG_ANY,
				"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": "
				"%s\n", text->bv_val, 0, 0 );
			e->e_id = NOID;
		}
		}

	} else {
		if (! (slapMode & SLAP_TOOL_QUICK)) {
		TXN_ABORT( tid );
		snprintf( text->bv_val, text->bv_len,
			"txn_aborted! %s (%d)",
			db_strerror(rc), rc );
		Debug( LDAP_DEBUG_ANY,
			"=> " LDAP_XSTRING(bdb_tool_entry_modify) ": %s\n",
			text->bv_val, 0, 0 );
		}
		e->e_id = NOID;
	}

	return e->e_id;
}
