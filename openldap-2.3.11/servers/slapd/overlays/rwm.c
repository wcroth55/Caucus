/* rwm.c - rewrite/remap operations */
/* $OpenLDAP: pkg/ldap/servers/slapd/overlays/rwm.c,v 1.37.2.12 2005/09/28 00:30:35 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2003-2005 The OpenLDAP Foundation.
 * Portions Copyright 2003 Pierangelo Masarati.
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

#ifdef SLAPD_OVER_RWM

#include <stdio.h>

#include <ac/string.h>

#include "slap.h"
#include "rwm.h"

static int
rwm_op_dn_massage( Operation *op, SlapReply *rs, void *cookie )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	struct berval		dn = BER_BVNULL,
				ndn = BER_BVNULL;
	int			rc = 0;
	dncookie		dc;

	/*
	 * Rewrite the dn if needed
	 */
	dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
	dc.conn = op->o_conn;
	dc.rs = rs;
	dc.ctx = (char *)cookie;
#else /* ! ENABLE_REWRITE */
	dc.tofrom = ((int *)cookie)[0];
	dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */

	/* NOTE: in those cases where only the ndn is available,
	 * and the caller sets op->o_req_dn = op->o_req_ndn,
	 * only rewrite the op->o_req_ndn and use it as 
	 * op->o_req_dn as well */
	ndn = op->o_req_ndn;
	if ( op->o_req_dn.bv_val != op->o_req_ndn.bv_val ) {
		dn = op->o_req_dn;
		rc = rwm_dn_massage_pretty_normalize( &dc, &op->o_req_dn, &dn, &ndn );
	} else {
		rc = rwm_dn_massage_normalize( &dc, &op->o_req_ndn, &ndn );
	}

	if ( rc != LDAP_SUCCESS ) {
		return rc;
	}

	if ( ( op->o_req_dn.bv_val != op->o_req_ndn.bv_val && dn.bv_val == op->o_req_dn.bv_val )
			|| ndn.bv_val == op->o_req_ndn.bv_val )
	{
		return LDAP_SUCCESS;
	}

	if ( op->o_req_dn.bv_val != op->o_req_ndn.bv_val ) {
		op->o_tmpfree( op->o_req_dn.bv_val, op->o_tmpmemctx );
		op->o_req_dn = dn;
	} else {
		op->o_req_dn = ndn;
	}
	op->o_tmpfree( op->o_req_ndn.bv_val, op->o_tmpmemctx );
	op->o_req_ndn = ndn;

	return LDAP_SUCCESS;
}

static int
rwm_op_add( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int			rc,
				i;
	Attribute		**ap = NULL;
	char			*olddn = op->o_req_dn.bv_val;
	int			isupdate;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "addDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "addDN massage error" );
		return -1;
	}

	if ( olddn != op->o_req_dn.bv_val ) {
		ber_bvreplace( &op->ora_e->e_name, &op->o_req_dn );
		ber_bvreplace( &op->ora_e->e_nname, &op->o_req_ndn );
	}

	/* Count number of attributes in entry */ 
	isupdate = be_shadow_update( op );
	for ( i = 0, ap = &op->oq_add.rs_e->e_attrs; *ap; ) {
		Attribute	*a;

		if ( (*ap)->a_desc == slap_schema.si_ad_objectClass ||
				(*ap)->a_desc == slap_schema.si_ad_structuralObjectClass )
		{
			int		j, last;

			for ( last = 0; !BER_BVISNULL( &(*ap)->a_vals[ last ] ); last++ )
					/* count values */ ;
			last--;
			for ( j = 0; !BER_BVISNULL( &(*ap)->a_vals[ j ] ); j++ ) {
				struct ldapmapping	*mapping = NULL;

				( void )rwm_mapping( &rwmap->rwm_oc, &(*ap)->a_vals[ j ],
						&mapping, RWM_MAP );
				if ( mapping == NULL ) {
					if ( rwmap->rwm_at.drop_missing ) {
						/* FIXME: we allow to remove objectClasses as well;
						 * if the resulting entry is inconsistent, that's
						 * the relayed database's business...
						 */
						ch_free( (*ap)->a_vals[ j ].bv_val );
						if ( last > j ) {
							(*ap)->a_vals[ j ] = (*ap)->a_vals[ last ];
						}
						BER_BVZERO( &(*ap)->a_vals[ last ] );
						last--;
						j--;
					}

				} else {
					ch_free( (*ap)->a_vals[ j ].bv_val );
					ber_dupbv( &(*ap)->a_vals[ j ], &mapping->m_dst );
				}
			}

		} else if ( !isupdate && !get_manageDIT( op ) && (*ap)->a_desc->ad_type->sat_no_user_mod )
		{
			goto next_attr;

		} else {
			struct ldapmapping	*mapping = NULL;

			( void )rwm_mapping( &rwmap->rwm_at, &(*ap)->a_desc->ad_cname,
					&mapping, RWM_MAP );
			if ( mapping == NULL ) {
				if ( rwmap->rwm_at.drop_missing ) {
					goto cleanup_attr;
				}
			}

			if ( (*ap)->a_desc->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName
					|| ( mapping != NULL && mapping->m_dst_ad->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName ) )
			{
				/*
				 * FIXME: rewrite could fail; in this case
				 * the operation should give up, right?
				 */
#ifdef ENABLE_REWRITE
				rc = rwm_dnattr_rewrite( op, rs, "addAttrDN",
						(*ap)->a_vals,
						(*ap)->a_nvals ? &(*ap)->a_nvals : NULL );
#else /* ! ENABLE_REWRITE */
				rc = 1;
				rc = rwm_dnattr_rewrite( op, rs, &rc, (*ap)->a_vals,
						(*ap)->a_nvals ? &(*ap)->a_nvals : NULL );
#endif /* ! ENABLE_REWRITE */
				if ( rc ) {
					goto cleanup_attr;
				}

			} else if ( (*ap)->a_desc == slap_schema.si_ad_ref ) {
#ifdef ENABLE_REWRITE
				rc = rwm_referral_rewrite( op, rs, "referralAttrDN",
						(*ap)->a_vals,
						(*ap)->a_nvals ? &(*ap)->a_nvals : NULL );
#else /* ! ENABLE_REWRITE */
				rc = 1;
				rc = rwm_referral_rewrite( op, rs, &rc, (*ap)->a_vals,
						(*ap)->a_nvals ? &(*ap)->a_nvals : NULL );
#endif /* ! ENABLE_REWRITE */
				if ( rc != LDAP_SUCCESS ) {
					goto cleanup_attr;
				}
			}
		
			if ( mapping != NULL ) {
				assert( mapping->m_dst_ad != NULL );
				(*ap)->a_desc = mapping->m_dst_ad;
			}
		}

next_attr:;
		ap = &(*ap)->a_next;
		continue;

cleanup_attr:;
		/* FIXME: leaking attribute/values? */
		a = *ap;

		*ap = (*ap)->a_next;
		attr_free( a );
	}

	/* TODO: map attribute types, values of DN-valued attributes ... */
	return SLAP_CB_CONTINUE;
}

#ifdef ENABLE_REWRITE
static int
rwm_conn_init( BackendDB *be, Connection *conn )
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	( void )rewrite_session_init( rwmap->rwm_rw, conn );

	return SLAP_CB_CONTINUE;
}

static int
rwm_conn_destroy( BackendDB *be, Connection *conn )
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	( void )rewrite_session_delete( rwmap->rwm_rw, conn );

	return SLAP_CB_CONTINUE;
}
#endif /* ENABLE_REWRITE */

static int
rwm_op_bind( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	int			rc;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "bindDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "bindDN massage error" );
		return -1;
	}

	return SLAP_CB_CONTINUE;
}

static int
rwm_op_unbind( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

#ifdef ENABLE_REWRITE
	rewrite_session_delete( rwmap->rwm_rw, op->o_conn );
#endif /* ENABLE_REWRITE */

	return SLAP_CB_CONTINUE;
}

static int
rwm_op_compare( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int			rc;
	struct berval		mapped_at = BER_BVNULL,
				mapped_vals[2] = { BER_BVNULL, BER_BVNULL };

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "compareDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "compareDN massage error" );
		return -1;
	}

	/* if the attribute is an objectClass, try to remap its value */
	if ( op->orc_ava->aa_desc == slap_schema.si_ad_objectClass
			|| op->orc_ava->aa_desc == slap_schema.si_ad_structuralObjectClass )
	{
		rwm_map( &rwmap->rwm_oc, &op->orc_ava->aa_value,
				&mapped_vals[0], RWM_MAP );
		if ( BER_BVISNULL( &mapped_vals[0] ) || BER_BVISEMPTY( &mapped_vals[0] ) )
		{
			op->o_bd->bd_info = (BackendInfo *)on->on_info;
			send_ldap_error( op, rs, LDAP_OTHER, "compare objectClass map error" );
			return -1;

		} else if ( mapped_vals[0].bv_val != op->orc_ava->aa_value.bv_val ) {
			ber_bvreplace_x( &op->orc_ava->aa_value, &mapped_vals[0], op->o_tmpmemctx );
		}
		mapped_at = op->orc_ava->aa_desc->ad_cname;

	} else {
		struct ldapmapping	*mapping = NULL;
		AttributeDescription	*ad = op->orc_ava->aa_desc;

		( void )rwm_mapping( &rwmap->rwm_at, &op->orc_ava->aa_desc->ad_cname,
				&mapping, RWM_MAP );
		if ( mapping == NULL ) {
			if ( rwmap->rwm_at.drop_missing ) {
				op->o_bd->bd_info = (BackendInfo *)on->on_info;
				send_ldap_error( op, rs, LDAP_OTHER, "compare attributeType map error" );
				return -1;
			}

		} else {
			assert( mapping->m_dst_ad != NULL );
			ad = mapping->m_dst_ad;
		}

		if ( op->orc_ava->aa_desc->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName
				|| ( mapping != NULL && mapping->m_dst_ad->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName ) )
		{
			struct berval	*mapped_valsp[2];
			
			mapped_valsp[0] = &mapped_vals[0];
			mapped_valsp[1] = &mapped_vals[1];

			mapped_vals[0] = op->orc_ava->aa_value;

#ifdef ENABLE_REWRITE
			rc = rwm_dnattr_rewrite( op, rs, "compareAttrDN", NULL, mapped_valsp );
#else /* ! ENABLE_REWRITE */
			rc = 1;
			rc = rwm_dnattr_rewrite( op, rs, &rc, NULL, mapped_valsp );
#endif /* ! ENABLE_REWRITE */

			if ( rc != LDAP_SUCCESS ) {
				op->o_bd->bd_info = (BackendInfo *)on->on_info;
				send_ldap_error( op, rs, rc, "compareAttrDN massage error" );
				return -1;
			}

			if ( mapped_vals[ 0 ].bv_val != op->orc_ava->aa_value.bv_val ) {
				/* NOTE: if we get here, rwm_dnattr_rewrite()
				 * already freed the old value, so now 
				 * it's invalid */
				ber_dupbv_x( &op->orc_ava->aa_value, &mapped_vals[0],
						op->o_tmpmemctx );
				ber_memfree_x( mapped_vals[ 0 ].bv_val, NULL );
			}
		}
		op->orc_ava->aa_desc = ad;
	}

	return SLAP_CB_CONTINUE;
}

static int
rwm_op_delete( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	int			rc;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "deleteDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "deleteDN massage error" );
		return -1;
	}

	return SLAP_CB_CONTINUE;
}

static int
rwm_op_modify( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int			isupdate;
	Modifications		**mlp;
	int			rc;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "modifyDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "modifyDN massage error" );
		return -1;
	}

	isupdate = be_shadow_update( op );
	for ( mlp = &op->oq_modify.rs_modlist; *mlp; ) {
		int			is_oc = 0;
		Modifications		*ml;
		struct ldapmapping	*mapping = NULL;

		if ( (*mlp)->sml_desc == slap_schema.si_ad_objectClass 
				|| (*mlp)->sml_desc == slap_schema.si_ad_structuralObjectClass )
		{
			is_oc = 1;

		} else if ( !isupdate && !get_manageDIT( op ) && (*mlp)->sml_desc->ad_type->sat_no_user_mod  )
		{
			goto next_mod;

		} else {
			int			drop_missing;

			drop_missing = rwm_mapping( &rwmap->rwm_at,
					&(*mlp)->sml_desc->ad_cname,
					&mapping, RWM_MAP );
			if ( drop_missing || ( mapping != NULL && BER_BVISNULL( &mapping->m_dst ) ) )
			{
				goto cleanup_mod;
			}
		}

		if ( (*mlp)->sml_values != NULL ) {
			if ( is_oc ) {
				int	last, j;

				for ( last = 0; !BER_BVISNULL( &(*mlp)->sml_values[ last ] ); last++ )
					/* count values */ ;
				last--;

				for ( j = 0; !BER_BVISNULL( &(*mlp)->sml_values[ j ] ); j++ ) {
					struct ldapmapping	*oc_mapping = NULL;
		
					( void )rwm_mapping( &rwmap->rwm_oc, &(*mlp)->sml_values[ j ],
							&oc_mapping, RWM_MAP );
					if ( oc_mapping == NULL ) {
						if ( rwmap->rwm_at.drop_missing ) {
							/* FIXME: we allow to remove objectClasses as well;
							 * if the resulting entry is inconsistent, that's
							 * the relayed database's business...
							 */
							ch_free( (*mlp)->sml_values[ j ].bv_val );
							if ( last > j ) {
								(*mlp)->sml_values[ j ] = (*mlp)->sml_values[ last ];
							}
							BER_BVZERO( &(*mlp)->sml_values[ last ] );
							last--;
							j--;
						}
	
					} else {
						ch_free( (*mlp)->sml_values[ j ].bv_val );
						ber_dupbv( &(*mlp)->sml_values[ j ], &oc_mapping->m_dst );
					}
				}

			} else {
				if ( (*mlp)->sml_desc->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName
						|| ( mapping != NULL && mapping->m_dst_ad->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName ) )
				{
#ifdef ENABLE_REWRITE
					rc = rwm_dnattr_rewrite( op, rs, "modifyAttrDN",
							(*mlp)->sml_values,
							(*mlp)->sml_nvalues ? &(*mlp)->sml_nvalues : NULL );
#else /* ! ENABLE_REWRITE */
					rc = 1;
					rc = rwm_dnattr_rewrite( op, rs, &rc, 
							(*mlp)->sml_values,
							(*mlp)->sml_nvalues ? &(*mlp)->sml_nvalues : NULL );
#endif /* ! ENABLE_REWRITE */

				} else if ( (*mlp)->sml_desc == slap_schema.si_ad_ref ) {
#ifdef ENABLE_REWRITE
					rc = rwm_referral_rewrite( op, rs,
							"referralAttrDN",
							(*mlp)->sml_values,
							(*mlp)->sml_nvalues ? &(*mlp)->sml_nvalues : NULL );
#else /* ! ENABLE_REWRITE */
					rc = 1;
					rc = rwm_referral_rewrite( op, rs, &rc,
							(*mlp)->sml_values,
							(*mlp)->sml_nvalues ? &(*mlp)->sml_nvalues : NULL );
#endif /* ! ENABLE_REWRITE */
					if ( rc != LDAP_SUCCESS ) {
						goto cleanup_mod;
					}
				}

				if ( rc != LDAP_SUCCESS ) {
					goto cleanup_mod;
				}
			}
		}

next_mod:;
		if ( mapping != NULL ) {
			/* use new attribute description */
			assert( mapping->m_dst_ad != NULL );
			(*mlp)->sml_desc = mapping->m_dst_ad;
		}

		mlp = &(*mlp)->sml_next;
		continue;

cleanup_mod:;
		ml = *mlp;
		*mlp = (*mlp)->sml_next;
		slap_mod_free( &ml->sml_mod, 0 );
		free( ml );
	}

	return SLAP_CB_CONTINUE;
}

static int
rwm_op_modrdn( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;
	
	int			rc;

	if ( op->orr_newSup ) {
		dncookie	dc;
		struct berval	nnewSup = BER_BVNULL;
		struct berval	newSup = BER_BVNULL;

		/*
		 * Rewrite the new superior, if defined and required
	 	 */
		dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
		dc.conn = op->o_conn;
		dc.rs = rs;
		dc.ctx = "newSuperiorDN";
#else /* ! ENABLE_REWRITE */
		dc.tofrom = 0;
		dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */
		newSup = *op->orr_newSup;
		nnewSup = *op->orr_nnewSup;
		rc = rwm_dn_massage_pretty_normalize( &dc, op->orr_newSup, &newSup, &nnewSup );
		if ( rc != LDAP_SUCCESS ) {
			op->o_bd->bd_info = (BackendInfo *)on->on_info;
			send_ldap_error( op, rs, rc, "newSuperiorDN massage error" );
			return -1;
		}

		if ( op->orr_newSup->bv_val != newSup.bv_val ) {
			op->o_tmpfree( op->orr_newSup->bv_val, op->o_tmpmemctx );
			op->o_tmpfree( op->orr_nnewSup->bv_val, op->o_tmpmemctx );
			*op->orr_newSup = newSup;
			*op->orr_nnewSup = nnewSup;
		}
	}

	/*
	 * Rewrite the dn, if needed
 	 */
#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "renameDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "renameDN massage error" );
		return -1;
	}

	/* TODO: rewrite newRDN, attribute types, 
	 * values of DN-valued attributes ... */
	return SLAP_CB_CONTINUE;
}

static slap_callback	rwm_cb;

static void
rwm_keyfree(
	void		*key,
	void		*data )
{
	ber_memfree_x( data, NULL );
}

static slap_callback *
rwm_callback_get( Operation *op )
{
	void		*data = NULL;

	if ( op->o_threadctx == NULL ) {
		return &rwm_cb;
	}

	ldap_pvt_thread_pool_getkey( op->o_threadctx,
			rwm_keyfree, &data, NULL );
	if ( data == NULL ) {
		data = ch_calloc( sizeof( slap_callback ), 1 );
		ldap_pvt_thread_pool_setkey( op->o_threadctx,
				rwm_keyfree, data, rwm_keyfree );
	}

	return (slap_callback *)data;
}

static int
rwm_swap_attrs( Operation *op, SlapReply *rs )
{
	slap_callback	*cb = op->o_callback;
	AttributeName	*an = (AttributeName *)cb->sc_private;

	rs->sr_attrs = an;
	
	return SLAP_CB_CONTINUE;
}

static int
rwm_op_search( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int			rc;
	dncookie		dc;

	struct berval		fstr = BER_BVNULL;
	Filter			*f = NULL;

	slap_callback		*cb = NULL;
	AttributeName		*an = NULL;

	char			*text = NULL;

#ifdef ENABLE_REWRITE
	rc = rewrite_session_var_set( rwmap->rwm_rw, op->o_conn,
		"searchFilter", op->ors_filterstr.bv_val );
	if ( rc == LDAP_SUCCESS )
		rc = rwm_op_dn_massage( op, rs, "searchDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		text = "searchDN massage error";
		goto error_return;
	}

	/*
	 * Rewrite the dn if needed
	 */
	dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
	dc.conn = op->o_conn;
	dc.rs = rs;
	dc.ctx = "searchFilterAttrDN";
#else /* ! ENABLE_REWRITE */
	dc.tofrom = 0;
	dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */

	rc = rwm_filter_map_rewrite( &dc, op->ors_filter, &fstr );
	if ( rc != LDAP_SUCCESS ) {
		text = "searchFilter/searchFilterAttrDN massage error";
		goto error_return;
	}

	f = str2filter_x( op, fstr.bv_val );

	if ( f == NULL ) {
		text = "massaged filter parse error";
		goto error_return;
	}

	if ( !BER_BVISNULL( &op->ors_filterstr ) ) {
		ch_free( op->ors_filterstr.bv_val );
	}

	if( op->ors_filter ) {
		filter_free_x( op, op->ors_filter );
	}

	op->ors_filter = f;
	op->ors_filterstr = fstr;

	rc = rwm_map_attrnames( &rwmap->rwm_at, &rwmap->rwm_oc,
			op->ors_attrs, &an, RWM_MAP );
	if ( rc != LDAP_SUCCESS ) {
		text = "attribute list mapping error";
		goto error_return;
	}

	cb = rwm_callback_get( op );

	cb->sc_response = rwm_swap_attrs;
	cb->sc_cleanup = NULL;
	cb->sc_private = (void *)op->ors_attrs;
	cb->sc_next = op->o_callback;

	op->o_callback = cb;
	op->ors_attrs = an;

	return SLAP_CB_CONTINUE;

error_return:;
	if ( an != NULL ) {
		ch_free( an );
	}

	if ( f != NULL ) {
		filter_free_x( op, f );
	}

	if ( !BER_BVISNULL( &fstr ) ) {
		ch_free( fstr.bv_val );
	}

	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	send_ldap_error( op, rs, rc, text );

	return -1;

}

static int
rwm_extended( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	int			rc;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "extendedDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "extendedDN massage error" );
		return -1;
	}

	/* TODO: rewrite/map extended data ? ... */
	return SLAP_CB_CONTINUE;
}

static int
rwm_matched( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	struct berval		dn, mdn;
	dncookie		dc;
	int			rc;

	if ( rs->sr_matched == NULL ) {
		return SLAP_CB_CONTINUE;
	}

	dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
	dc.conn = op->o_conn;
	dc.rs = rs;
	dc.ctx = "matchedDN";
#else /* ! ENABLE_REWRITE */
	dc.tofrom = 0;
	dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */
	ber_str2bv( rs->sr_matched, 0, 0, &dn );
	mdn = dn;
	rc = rwm_dn_massage_pretty( &dc, &dn, &mdn );
	if ( rc != LDAP_SUCCESS ) {
		rs->sr_err = rc;
		rs->sr_text = "Rewrite error";
		return 1;
	}

	if ( mdn.bv_val != dn.bv_val ) {
		if ( rs->sr_flags & REP_MATCHED_MUSTBEFREED ) {
			ch_free( (void *)rs->sr_matched );

		} else {
			rs->sr_flags |= REP_MATCHED_MUSTBEFREED;
		}
		rs->sr_matched = mdn.bv_val;
	}
	
	return SLAP_CB_CONTINUE;
}

static int
rwm_attrs( Operation *op, SlapReply *rs, Attribute** a_first, int stripEntryDN )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	dncookie		dc;
	int			rc;
	Attribute		**ap;
	int			isupdate;

	/*
	 * Rewrite the dn attrs, if needed
	 */
	dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
	dc.conn = op->o_conn;
	dc.rs = NULL; 
#else /* ! ENABLE_REWRITE */
	dc.tofrom = 0;
	dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */

	/* FIXME: the entries are in the remote mapping form;
	 * so we need to select those attributes we are willing
	 * to return, and remap them accordingly */

	/* FIXME: in principle, one could map an attribute
	 * on top of another, which already exists.
	 * As such, in the end there might exist more than
	 * one instance of an attribute.
	 * We should at least check if this occurs, and issue
	 * an error (because multiple instances of attrs in 
	 * response are not valid), or merge the values (what
	 * about duplicate values?) */
	isupdate = be_shadow_update( op );
	for ( ap = a_first; *ap; ) {
		struct ldapmapping	*mapping = NULL;
		int			drop_missing;
		int			last;
		Attribute		*a;

		if ( SLAP_OPATTRS( rs->sr_attr_flags ) && is_at_operational( (*ap)->a_desc->ad_type ) )
		{
			/* go on */ ;
			
		} else {
			if ( op->ors_attrs != NULL && 
					!SLAP_USERATTRS( rs->sr_attr_flags ) &&
					!ad_inlist( (*ap)->a_desc, op->ors_attrs ) )
			{
				goto cleanup_attr;
			}

			drop_missing = rwm_mapping( &rwmap->rwm_at,
					&(*ap)->a_desc->ad_cname, &mapping, RWM_REMAP );
			if ( drop_missing || ( mapping != NULL && BER_BVISEMPTY( &mapping->m_dst ) ) )
			{
				goto cleanup_attr;
			}

			if ( mapping != NULL ) {
				(*ap)->a_desc = mapping->m_dst_ad;
			}
		}

		if ( (*ap)->a_desc == slap_schema.si_ad_entryDN ) {
			if ( stripEntryDN ) {
				/* will be generated by frontend */
				goto cleanup_attr;
			}
			
		} else if ( !isupdate
			&& !get_manageDIT( op )
			&& (*ap)->a_desc->ad_type->sat_no_user_mod 
			&& (*ap)->a_desc->ad_type != slap_schema.si_at_undefined )
		{
			goto next_attr;
		}

		for ( last = 0; !BER_BVISNULL( &(*ap)->a_vals[last] ); last++ )
			/* just count */ ;

		if ( last == 0 ) {
			/* empty? leave it in place because of attrsonly and vlv */
			goto next_attr;
		}
		last--;

		if ( (*ap)->a_desc == slap_schema.si_ad_objectClass
				|| (*ap)->a_desc == slap_schema.si_ad_structuralObjectClass )
		{
			struct berval	*bv;
			
			for ( bv = (*ap)->a_vals; !BER_BVISNULL( bv ); bv++ ) {
				struct berval	mapped;

				rwm_map( &rwmap->rwm_oc, &bv[0], &mapped, RWM_REMAP );
				if ( BER_BVISNULL( &mapped ) || BER_BVISEMPTY( &mapped ) ) {
					ch_free( bv[0].bv_val );
					BER_BVZERO( &bv[0] );
					if ( &(*ap)->a_vals[last] > &bv[0] ) {
						bv[0] = (*ap)->a_vals[last];
						BER_BVZERO( &(*ap)->a_vals[last] );
					}
					last--;
					bv--;

				} else if ( mapped.bv_val != bv[0].bv_val ) {
					/*
					 * FIXME: after LBER_FREEing
					 * the value is replaced by
					 * ch_alloc'ed memory
					 */
					ber_bvreplace( &bv[0], &mapped );
				}
			}

		/*
		 * It is necessary to try to rewrite attributes with
		 * dn syntax because they might be used in ACLs as
		 * members of groups; since ACLs are applied to the
		 * rewritten stuff, no dn-based subject clause could
		 * be used at the ldap backend side (see
		 * http://www.OpenLDAP.org/faq/data/cache/452.html)
		 * The problem can be overcome by moving the dn-based
		 * ACLs to the target directory server, and letting
		 * everything pass thru the ldap backend. */
		/* FIXME: handle distinguishedName-like syntaxes, like
		 * nameAndOptionalUID */
		} else if ( (*ap)->a_desc->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName
				|| ( mapping != NULL && mapping->m_src_ad->ad_type->sat_syntax == slap_schema.si_syn_distinguishedName ) )
		{
#ifdef ENABLE_REWRITE
			dc.ctx = "searchAttrDN";
#endif /* ENABLE_REWRITE */
			rc = rwm_dnattr_result_rewrite( &dc, (*ap)->a_vals );
			if ( rc != LDAP_SUCCESS ) {
				goto cleanup_attr;
			}

		} else if ( (*ap)->a_desc == slap_schema.si_ad_ref ) {
#ifdef ENABLE_REWRITE
			dc.ctx = "searchAttrDN";
#endif /* ENABLE_REWRITE */
			rc = rwm_referral_result_rewrite( &dc, (*ap)->a_vals );
			if ( rc != LDAP_SUCCESS ) {
				goto cleanup_attr;
			}
		}

		if ( mapping != NULL ) {
			/* rewrite the attribute description */
			assert( mapping->m_dst_ad != NULL );
			(*ap)->a_desc = mapping->m_dst_ad;
		}

next_attr:;
		ap = &(*ap)->a_next;
		continue;

cleanup_attr:;
		a = *ap;
		*ap = (*ap)->a_next;

		attr_free( a );
	}

	return 0;
}

static int
rwm_send_entry( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	Entry			*e = NULL;
	slap_mask_t		flags;
	struct berval		dn = BER_BVNULL,
				ndn = BER_BVNULL;
	dncookie		dc;
	int			rc;

	assert( rs->sr_entry != NULL );

	/*
	 * Rewrite the dn of the result, if needed
	 */
	dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
	dc.conn = op->o_conn;
	dc.rs = NULL; 
	dc.ctx = "searchEntryDN";
#else /* ! ENABLE_REWRITE */
	dc.tofrom = 0;
	dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */

	e = rs->sr_entry;
	flags = rs->sr_flags;
	if ( !( rs->sr_flags & REP_ENTRY_MODIFIABLE ) ) {
		/* FIXME: all we need to duplicate are:
		 * - dn
		 * - ndn
		 * - attributes that are requested
		 * - no values if attrsonly is set
		 */

		e = entry_dup( e );
		if ( e == NULL ) {
			rc = LDAP_NO_MEMORY;
			goto fail;
		}

		flags |= ( REP_ENTRY_MODIFIABLE | REP_ENTRY_MUSTBEFREED );
	}

	/*
	 * Note: this may fail if the target host(s) schema differs
	 * from the one known to the meta, and a DN with unknown
	 * attributes is returned.
	 */
	dn = e->e_name;
	ndn = e->e_nname;
	rc = rwm_dn_massage_pretty_normalize( &dc, &e->e_name, &dn, &ndn );
	if ( rc != LDAP_SUCCESS ) {
		rc = 1;
		goto fail;
	}

	if ( e->e_name.bv_val != dn.bv_val ) {
		ch_free( e->e_name.bv_val );
		ch_free( e->e_nname.bv_val );

		e->e_name = dn;
		e->e_nname = ndn;
	}

	/* TODO: map entry attribute types, objectclasses 
	 * and dn-valued attribute values */

	/* FIXME: the entries are in the remote mapping form;
	 * so we need to select those attributes we are willing
	 * to return, and remap them accordingly */
	(void)rwm_attrs( op, rs, &e->e_attrs, 1 );

#if 0
	if ( rs->sr_operational_attrs ) {
		(void)rwm_attrs( op, rs, &rs->sr_operational_attrs, 0 );
	}
#endif

	rs->sr_entry = e;
	rs->sr_flags = flags;

	return SLAP_CB_CONTINUE;

fail:;
	if ( e != NULL && e != rs->sr_entry ) {
		if ( e->e_name.bv_val == dn.bv_val ) {
			BER_BVZERO( &e->e_name );
		}

		if ( e->e_nname.bv_val == ndn.bv_val ) {
			BER_BVZERO( &e->e_nname );
		}

		entry_free( e );
	}

	if ( !BER_BVISNULL( &dn ) ) {
		ch_free( dn.bv_val );
	}

	if ( !BER_BVISNULL( &ndn ) ) {
		ch_free( ndn.bv_val );
	}

	return rc;
}

static int
rwm_operational( Operation *op, SlapReply *rs )
{
	/* FIXME: the entries are in the remote mapping form;
	 * so we need to select those attributes we are willing
	 * to return, and remap them accordingly */
	if ( rs->sr_operational_attrs ) {
		rwm_attrs( op, rs, &rs->sr_operational_attrs, 1 );
	}

	return SLAP_CB_CONTINUE;
}

#if 0
/* don't use this; it cannot be reverted, and leaves op->o_req_dn
 * rewritten for subsequent operations; fine for plain suffixmassage,
 * but destroys everything else */
static int
rwm_chk_referrals( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *) op->o_bd->bd_info;
	int			rc;

#ifdef ENABLE_REWRITE
	rc = rwm_op_dn_massage( op, rs, "referralCheckDN" );
#else /* ! ENABLE_REWRITE */
	rc = 1;
	rc = rwm_op_dn_massage( op, rs, &rc );
#endif /* ! ENABLE_REWRITE */
	if ( rc != LDAP_SUCCESS ) {
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, rc, "referralCheckDN massage error" );
		return -1;
	}

	return SLAP_CB_CONTINUE;
}
#endif

static int
rwm_rw_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv
)
{
#ifdef ENABLE_REWRITE
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	return rewrite_parse( rwmap->rwm_rw,
				fname, lineno, argc, argv );

#else /* !ENABLE_REWRITE */
	fprintf( stderr, "%s: line %d: rewrite capabilities "
			"are not enabled\n", fname, lineno );
#endif /* !ENABLE_REWRITE */
		
	return 0;
}

static int
rwm_suffixmassage_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv
)
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	struct berval		bvnc, nvnc, pvnc, brnc, nrnc, prnc;
	int			massaged;
#ifdef ENABLE_REWRITE
	int			rc;
#endif /* ENABLE_REWRITE */
		
	/*
	 * syntax:
	 * 
	 * 	suffixmassage [<suffix>] <massaged suffix>
	 *
	 * the [<suffix>] field must be defined as a valid suffix
	 * for the current database;
	 * the <massaged suffix> shouldn't have already been
	 * defined as a valid suffix for the current server
	 */
	if ( argc == 2 ) {
		if ( be->be_suffix == NULL ) {
 			fprintf( stderr, "%s: line %d: "
				       " \"suffixMassage [<suffix>]"
				       " <massaged suffix>\" without "
				       "<suffix> part requires database "
				       "suffix be defined first.\n",
				fname, lineno );
			return 1;
		}
		bvnc = be->be_suffix[ 0 ];
		massaged = 1;

	} else if ( argc == 3 ) {
		ber_str2bv( argv[ 1 ], 0, 0, &bvnc );
		massaged = 2;

	} else  {
 		fprintf( stderr, "%s: line %d: syntax is"
			       " \"suffixMassage [<suffix>]"
			       " <massaged suffix>\"\n",
			fname, lineno );
		return 1;
	}

	if ( dnPrettyNormal( NULL, &bvnc, &pvnc, &nvnc, NULL ) != LDAP_SUCCESS ) {
		fprintf( stderr, "%s: line %d: suffix DN %s is invalid\n",
			fname, lineno, bvnc.bv_val );
		return 1;
	}

	ber_str2bv( argv[ massaged ], 0, 0, &brnc );
	if ( dnPrettyNormal( NULL, &brnc, &prnc, &nrnc, NULL ) != LDAP_SUCCESS ) {
		fprintf( stderr, "%s: line %d: suffix DN %s is invalid\n",
				fname, lineno, brnc.bv_val );
		free( nvnc.bv_val );
		free( pvnc.bv_val );
		return 1;
	}

#ifdef ENABLE_REWRITE
	/*
	 * The suffix massaging is emulated 
	 * by means of the rewrite capabilities
	 */
 	rc = rwm_suffix_massage_config( rwmap->rwm_rw,
			&pvnc, &nvnc, &prnc, &nrnc );
	free( nvnc.bv_val );
	free( pvnc.bv_val );
	free( nrnc.bv_val );
	free( prnc.bv_val );

	return( rc );

#else /* !ENABLE_REWRITE */
	ber_bvarray_add( &rwmap->rwm_suffix_massage, &pvnc );
	ber_bvarray_add( &rwmap->rwm_suffix_massage, &nvnc );
		
	ber_bvarray_add( &rwmap->rwm_suffix_massage, &prnc );
	ber_bvarray_add( &rwmap->rwm_suffix_massage, &nrnc );
#endif /* !ENABLE_REWRITE */

	return 0;
}

static int
rwm_m_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv
)
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	/* objectclass/attribute mapping */
	return rwm_map_config( &rwmap->rwm_oc,
			&rwmap->rwm_at,
			fname, lineno, argc, argv );
}

static int
rwm_response( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *)op->o_bd->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int		rc;

	if ( op->o_tag == LDAP_REQ_SEARCH && rs->sr_type == REP_SEARCH ) {
		return rwm_send_entry( op, rs );
	}

	switch( op->o_tag ) {
	case LDAP_REQ_SEARCH:
		/* Note: the operation attrs are remapped */
		if ( rs->sr_type == REP_RESULT
				&& op->ors_attrs != NULL
				&& op->ors_attrs != rs->sr_attrs )
		{
			ch_free( op->ors_attrs );
			op->ors_attrs = rs->sr_attrs;
		}
		/* fall thru */

	case LDAP_REQ_BIND:
	case LDAP_REQ_ADD:
	case LDAP_REQ_DELETE:
	case LDAP_REQ_MODRDN:
	case LDAP_REQ_MODIFY:
	case LDAP_REQ_COMPARE:
	case LDAP_REQ_EXTENDED:
		if ( rs->sr_ref ) {
			dncookie		dc;

			/*
			 * Rewrite the dn of the referrals, if needed
			 */
			dc.rwmap = rwmap;
#ifdef ENABLE_REWRITE
			dc.conn = op->o_conn;
			dc.rs = NULL; 
			dc.ctx = "referralDN";
#else /* ! ENABLE_REWRITE */
			dc.tofrom = 0;
			dc.normalized = 0;
#endif /* ! ENABLE_REWRITE */
			rc = rwm_referral_result_rewrite( &dc, rs->sr_ref );
			if ( rc != LDAP_SUCCESS ) {
				rc = 1;
				break;
			}
		}
		rc = rwm_matched( op, rs );
		break;

	default:
		rc = SLAP_CB_CONTINUE;
		break;
	}

	return rc;
}

static int
rwm_db_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv
)
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

	int		rc = 0;
	char		*argv0 = NULL;

	if ( strncasecmp( argv[ 0 ], "rwm-", STRLENOF( "rwm-" ) ) == 0 ) {
		argv0 = argv[ 0 ];
		argv[ 0 ] = &argv0[ STRLENOF( "rwm-" ) ];
	}

	if ( strncasecmp( argv[0], "rewrite", STRLENOF("rewrite") ) == 0 ) {
		rc = rwm_rw_config( be, fname, lineno, argc, argv );

	} else if ( strcasecmp( argv[0], "map" ) == 0 ) {
		rc = rwm_m_config( be, fname, lineno, argc, argv );

	} else if ( strcasecmp( argv[0], "suffixmassage" ) == 0 ) {
		rc = rwm_suffixmassage_config( be, fname, lineno, argc, argv );

	} else if ( strcasecmp( argv[0], "t-f-support" ) == 0 ) {
		if ( argc != 2 ) {
			fprintf( stderr,
		"%s: line %d: \"t-f-support {no|yes|discover}\" needs 1 argument.\n",
					fname, lineno );
			return( 1 );
		}

		if ( strcasecmp( argv[ 1 ], "no" ) == 0 ) {
			rwmap->rwm_flags &= ~(RWM_F_SUPPORT_T_F|RWM_F_SUPPORT_T_F_DISCOVER);

		} else if ( strcasecmp( argv[ 1 ], "yes" ) == 0 ) {
			rwmap->rwm_flags |= RWM_F_SUPPORT_T_F;

#if 0
		/* TODO: not implemented yet */
		} else if ( strcasecmp( argv[ 1 ], "discover" ) == 0 ) {
			rwmap->rwm_flags |= RWM_F_SUPPORT_T_F_DISCOVER;
#endif

		} else {
			fprintf( stderr,
	"%s: line %d: unknown value \"%s\" for \"t-f-support {no|yes|discover}\".\n",
				fname, lineno, argv[ 1 ] );
			return 1;
		}

	} else {
		rc = SLAP_CONF_UNKNOWN;
	}

	if ( argv0 ) {
		argv[ 0 ] = argv0;
	}

	return rc;
}

static int
rwm_db_init(
	BackendDB *be
)
{
	slap_overinst		*on = (slap_overinst *) be->bd_info;
	struct ldapmapping	*mapping = NULL;
	struct ldaprwmap	*rwmap;
#ifdef ENABLE_REWRITE
	char			*rargv[ 3 ];
#endif /* ENABLE_REWRITE */

	rwmap = (struct ldaprwmap *)ch_calloc( 1, sizeof( struct ldaprwmap ) );

#ifdef ENABLE_REWRITE
 	rwmap->rwm_rw = rewrite_info_init( REWRITE_MODE_USE_DEFAULT );
	if ( rwmap->rwm_rw == NULL ) {
 		ch_free( rwmap );
 		return -1;
 	}

	/* this rewriteContext by default must be null;
	 * rules can be added if required */
	rargv[ 0 ] = "rewriteContext";
	rargv[ 1 ] = "searchFilter";
	rargv[ 2 ] = NULL;
	rewrite_parse( rwmap->rwm_rw, "<suffix massage>", 1, 2, rargv );

	rargv[ 0 ] = "rewriteContext";
	rargv[ 1 ] = "default";
	rargv[ 2 ] = NULL;
	rewrite_parse( rwmap->rwm_rw, "<suffix massage>", 2, 2, rargv );
#endif /* ENABLE_REWRITE */

	if ( rwm_map_init( &rwmap->rwm_oc, &mapping ) != LDAP_SUCCESS ||
			rwm_map_init( &rwmap->rwm_at, &mapping ) != LDAP_SUCCESS )
	{
		return 1;
	}

	on->on_bi.bi_private = (void *)rwmap;

	return 0;
}

static int
rwm_db_destroy(
	BackendDB *be
)
{
	slap_overinst	*on = (slap_overinst *) be->bd_info;
	int		rc = 0;

	if ( on->on_bi.bi_private ) {
		struct ldaprwmap	*rwmap = 
			(struct ldaprwmap *)on->on_bi.bi_private;

#ifdef ENABLE_REWRITE
		if (rwmap->rwm_rw) {
			rewrite_info_delete( &rwmap->rwm_rw );
		}
#else /* !ENABLE_REWRITE */
		if ( rwmap->rwm_suffix_massage ) {
  			ber_bvarray_free( rwmap->rwm_suffix_massage );
 		}
#endif /* !ENABLE_REWRITE */

		avl_free( rwmap->rwm_oc.remap, NULL );
		avl_free( rwmap->rwm_oc.map, rwm_mapping_free );
		avl_free( rwmap->rwm_at.remap, NULL );
		avl_free( rwmap->rwm_at.map, rwm_mapping_free );

		ch_free( rwmap );
	}

	return rc;
}

static slap_overinst rwm = { { NULL } };

int
rwm_init(void)
{
	memset( &rwm, 0, sizeof( slap_overinst ) );

	rwm.on_bi.bi_type = "rwm";

	rwm.on_bi.bi_db_init = rwm_db_init;
	rwm.on_bi.bi_db_config = rwm_db_config;
	rwm.on_bi.bi_db_destroy = rwm_db_destroy;

	rwm.on_bi.bi_op_bind = rwm_op_bind;
	rwm.on_bi.bi_op_search = rwm_op_search;
	rwm.on_bi.bi_op_compare = rwm_op_compare;
	rwm.on_bi.bi_op_modify = rwm_op_modify;
	rwm.on_bi.bi_op_modrdn = rwm_op_modrdn;
	rwm.on_bi.bi_op_add = rwm_op_add;
	rwm.on_bi.bi_op_delete = rwm_op_delete;
	rwm.on_bi.bi_op_unbind = rwm_op_unbind;
	rwm.on_bi.bi_extended = rwm_extended;

	rwm.on_bi.bi_operational = rwm_operational;
	rwm.on_bi.bi_chk_referrals = 0 /* rwm_chk_referrals */ ;

#ifdef ENABLE_REWRITE
	rwm.on_bi.bi_connection_init = rwm_conn_init;
	rwm.on_bi.bi_connection_destroy = rwm_conn_destroy;
#endif /* ENABLE_REWRITE */

	rwm.on_response = rwm_response;

	return overlay_register( &rwm );
}

#if SLAPD_OVER_RWM == SLAPD_MOD_DYNAMIC
int
init_module( int argc, char *argv[] )
{
	return rwm_init();
}
#endif /* SLAPD_OVER_RWM == SLAPD_MOD_DYNAMIC */

#endif /* SLAPD_OVER_RWM */
