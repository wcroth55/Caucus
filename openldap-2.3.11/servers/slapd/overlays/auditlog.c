/* auditlog.c - log modifications for audit/history purposes */
/* $OpenLDAP: pkg/ldap/servers/slapd/overlays/auditlog.c,v 1.1.2.1 2005/06/08 21:50:17 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2005 The OpenLDAP Foundation.
 * Portions copyright 2004-2005 Symas Corporation.
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
/* ACKNOWLEDGEMENTS:
 * This work was initially developed by Symas Corp. for inclusion in
 * OpenLDAP Software.  This work was sponsored by Hewlett-Packard.
 */

#include "portable.h"

#ifdef SLAPD_OVER_AUDITLOG

#include <stdio.h>

#include <ac/string.h>
#include <ac/ctype.h>

#include "slap.h"
#include "ldif.h"

typedef struct auditlog_data {
	ldap_pvt_thread_mutex_t ad_mutex;
	char *ad_logfile;
} auditlog_data;

int fprint_ldif(FILE *f, char *name, char *val, ber_len_t len) {
	char *s;
	if((s = ldif_put(LDIF_PUT_VALUE, name, val, len)) == NULL)
		return(-1);
	fputs(s, f);
	ber_memfree(s);
	return(0);
}

int auditlog_response(Operation *op, SlapReply *rs) {
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;
	auditlog_data *ad = on->on_bi.bi_private;
	FILE *f;
	Attribute *a;
	Modifications *m;
	struct berval *b;
	char *what, *subop, *suffix, *who = NULL;
	long stamp = slap_get_time();
	int i;

	if ( rs->sr_err != LDAP_SUCCESS ) return SLAP_CB_CONTINUE;

	if ( !op->o_bd || !ad->ad_logfile ) return SLAP_CB_CONTINUE;

/*
** add or modify: use modifiersName if present
**
*/
	switch(op->o_tag) {
		case LDAP_REQ_MODRDN:	what = "modrdn";	break;
		case LDAP_REQ_DELETE:	what = "delete";	break;
		case LDAP_REQ_ADD:
			what = "add";
			for(a = op->ora_e->e_attrs; a; a = a->a_next)
				if( a->a_desc == slap_schema.si_ad_modifiersName ) {
					who = a->a_vals[0].bv_val;
					break;
				}
			break;
		case LDAP_REQ_MODIFY:
			what = "modify";
			for(m = op->orm_modlist; m; m = m->sml_next)
				if( m->sml_desc == slap_schema.si_ad_modifiersName ) {
					who = m->sml_values[0].bv_val;
					break;
				}
			break;
		default:
			return SLAP_CB_CONTINUE;
	}

	suffix = op->o_bd->be_suffix[0].bv_len ? op->o_bd->be_suffix[0].bv_val :
		"global";

/*
** note: this means requestor's dn when modifiersName is null
*/
	if ( !who )
		who = op->o_dn.bv_val;

	ldap_pvt_thread_mutex_lock(&ad->ad_mutex);
	if((f = fopen(ad->ad_logfile, "a")) == NULL) {
		ldap_pvt_thread_mutex_unlock(&ad->ad_mutex);
		return SLAP_CB_CONTINUE;
	}

	fprintf(f, "# %s %ld %s%s%s\ndn: %s\nchangetype: %s\n",
		what, stamp, suffix, who ? " " : "", who ? who : "",
		op->o_req_dn.bv_val, what);

	switch(op->o_tag) {
	  case LDAP_REQ_ADD:
		for(a = op->ora_e->e_attrs; a; a = a->a_next)
		    if(b = a->a_vals)
			for(i = 0; b[i].bv_val; i++)
				fprint_ldif(f, a->a_desc->ad_cname.bv_val, b[i].bv_val, b[i].bv_len);
		break;

	  case LDAP_REQ_MODIFY:
		for(m = op->orm_modlist; m; m = m->sml_next) {
			switch(m->sml_op & LDAP_MOD_OP) {
				case LDAP_MOD_ADD:	 what = "add";		break;
				case LDAP_MOD_REPLACE:	 what = "replace";	break;
				case LDAP_MOD_DELETE:	 what = "delete";	break;
				case LDAP_MOD_INCREMENT: what = "increment";	break;
				default:
					fprintf(f, "# MOD_TYPE_UNKNOWN:%02x\n", m->sml_op & LDAP_MOD_OP);
					continue;
			}
			fprintf(f, "%s: %s\n", what, m->sml_desc->ad_cname.bv_val);
			if(b = m->sml_values) for(i = 0; b[i].bv_val; i++)
				fprint_ldif(f, m->sml_desc->ad_cname.bv_val, b[i].bv_val, b[i].bv_len);
			fprintf(f, "-\n");
		}
		break;

	  case LDAP_REQ_MODRDN:
		fprintf(f, "newrdn: %s\ndeleteoldrdn: %s\n",
			op->orr_newrdn.bv_val, op->orr_deleteoldrdn ? "1" : "0");
		if(op->orr_newSup) fprintf(f, "newsuperior: %s\n", op->orr_newSup->bv_val);
		break;

	  case LDAP_REQ_DELETE:
		/* nothing else needed */
		break;
	}

	fprintf(f, "# end %s %ld\n\n", what, stamp);

	fclose(f);
	ldap_pvt_thread_mutex_unlock(&ad->ad_mutex);
	return SLAP_CB_CONTINUE;
}

static slap_overinst auditlog;

static int
auditlog_db_init(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	auditlog_data *ad = ch_malloc(sizeof(auditlog_data));

	on->on_bi.bi_private = ad;
	ldap_pvt_thread_mutex_init( &ad->ad_mutex );
	return 0;
}

static int
auditlog_db_close(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	auditlog_data *ad = on->on_bi.bi_private;

	free( ad->ad_logfile );
	ad->ad_logfile = NULL;
}

static int
auditlog_db_destroy(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *)be->bd_info;
	auditlog_data *ad = on->on_bi.bi_private;

	ldap_pvt_thread_mutex_destroy( &ad->ad_mutex );
	free( ad );
}

static int
auditlog_config(
	BackendDB	*be,
	const char	*fname,
	int		lineno,
	int		argc,
	char	**argv
)
{
	slap_overinst *on = (slap_overinst *) be->bd_info;
	auditlog_data *ad = on->on_bi.bi_private;

	/* history log file */
	if ( strcasecmp( argv[0], "auditlog" ) == 0 ) {
		if ( argc < 2 ) {
			Debug( LDAP_DEBUG_ANY,
	    "%s: line %d: missing filename in \"auditlog <filename>\" line\n",
			    fname, lineno, 0 );
				return( 1 );
		}
		ad->ad_logfile = ch_strdup( argv[1] );
		return 0;
	}
	return SLAP_CONF_UNKNOWN;
}

int auditlog_init() {

	auditlog.on_bi.bi_type = "auditlog";
	auditlog.on_bi.bi_db_init = auditlog_db_init;
	auditlog.on_bi.bi_db_config = auditlog_config;
	auditlog.on_bi.bi_db_close = auditlog_db_close;
	auditlog.on_bi.bi_db_destroy = auditlog_db_destroy;
	auditlog.on_response = auditlog_response;

	return overlay_register(&auditlog);
}

#if SLAPD_OVER_AUDITLOG == SLAPD_MOD_DYNAMIC && defined(PIC)
int init_module( int argc, char *argv[]) {
	return auditlog_init();
}
#endif

#endif /* SLAPD_OVER_AUDITLOG */
