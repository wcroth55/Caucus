/* operation.c - routines to deal with pending ldap operations */
/* $OpenLDAP: pkg/ldap/servers/slapd/operation.c,v 1.63.2.4 2005/08/09 21:03:59 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2005 The OpenLDAP Foundation.
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
/* Portions Copyright (c) 1995 Regents of the University of Michigan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of Michigan at Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#include "portable.h"

#include <stdio.h>

#include <ac/string.h>
#include <ac/socket.h>

#include "slap.h"

#ifdef LDAP_SLAPI
#include "slapi/slapi.h"
#endif

static ldap_pvt_thread_mutex_t	slap_op_mutex;
static LDAP_STAILQ_HEAD(s_o, slap_op)	slap_free_ops;
static time_t last_time;
static int last_incr;

void slap_op_init(void)
{
	ldap_pvt_thread_mutex_init( &slap_op_mutex );
	LDAP_STAILQ_INIT(&slap_free_ops);
}

void slap_op_destroy(void)
{
	Operation *o;

	while ( (o = LDAP_STAILQ_FIRST( &slap_free_ops )) != NULL) {
		LDAP_STAILQ_REMOVE_HEAD( &slap_free_ops, o_next );
		LDAP_STAILQ_NEXT(o, o_next) = NULL;
		ch_free( o );
	}
	ldap_pvt_thread_mutex_destroy( &slap_op_mutex );
}

void
slap_op_free( Operation *op )
{
	assert( LDAP_STAILQ_NEXT(op, o_next) == NULL );

	if ( op->o_ber != NULL ) {
		ber_free( op->o_ber, 1 );
	}
	if ( !BER_BVISNULL( &op->o_dn ) ) {
		ch_free( op->o_dn.bv_val );
	}
	if ( !BER_BVISNULL( &op->o_ndn ) ) {
		ch_free( op->o_ndn.bv_val );
	}
	if ( !BER_BVISNULL( &op->o_authmech ) ) {
		ch_free( op->o_authmech.bv_val );
	}
	if ( op->o_ctrls != NULL ) {
		slap_free_ctrls( op, op->o_ctrls );
	}

#ifdef LDAP_CONNECTIONLESS
	if ( op->o_res_ber != NULL ) {
		ber_free( op->o_res_ber, 1 );
	}
#endif

	{
		GroupAssertion *g, *n;
		for ( g = op->o_groups; g; g = n ) {
			n = g->ga_next;
			slap_sl_free( g, op->o_tmpmemctx );
		}
		op->o_groups = NULL;
	}

#if defined( LDAP_SLAPI )
	if ( slapi_plugins_used ) {
		slapi_int_free_object_extensions( SLAPI_X_EXT_OPERATION, op );
	}
#endif /* defined( LDAP_SLAPI ) */


	memset( op, 0, sizeof(Operation) + sizeof(Opheader) + SLAP_MAX_CIDS * sizeof(void *) );
	op->o_hdr = (Opheader *)(op+1);
	op->o_controls = (void **)(op->o_hdr+1);

	ldap_pvt_thread_mutex_lock( &slap_op_mutex );
	LDAP_STAILQ_INSERT_HEAD( &slap_free_ops, op, o_next );
	ldap_pvt_thread_mutex_unlock( &slap_op_mutex );
}

Operation *
slap_op_alloc(
    BerElement		*ber,
    ber_int_t	msgid,
    ber_tag_t	tag,
    ber_int_t	id
)
{
	Operation	*op;

	ldap_pvt_thread_mutex_lock( &slap_op_mutex );
	if ((op = LDAP_STAILQ_FIRST( &slap_free_ops ))) {
		LDAP_STAILQ_REMOVE_HEAD( &slap_free_ops, o_next );
	}
	ldap_pvt_thread_mutex_unlock( &slap_op_mutex );

	if (!op) {
		op = (Operation *) ch_calloc( 1, sizeof(Operation)
			+ sizeof(Opheader) + SLAP_MAX_CIDS * sizeof(void *) );
		op->o_hdr = (Opheader *)(op + 1);
		op->o_controls = (void **)(op->o_hdr+1);
	}

	op->o_ber = ber;
	op->o_msgid = msgid;
	op->o_tag = tag;

	op->o_time = slap_get_time();
	if ( op->o_time == last_time ) {
		op->o_tincr = ++last_incr;
	} else {
		last_time = op->o_time;
		last_incr = 0;	/* o_tincr is alredy zero */
	}
	op->o_opid = id;
	op->o_res_ber = NULL;

#if defined( LDAP_SLAPI )
	if ( slapi_plugins_used ) {
		slapi_int_create_object_extensions( SLAPI_X_EXT_OPERATION, op );
	}
#endif /* defined( LDAP_SLAPI ) */

	return( op );
}
