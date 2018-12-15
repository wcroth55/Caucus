/* $OpenLDAP: pkg/ldap/servers/slapd/back-perl/modify.c,v 1.21.2.1 2005/01/20 18:04:01 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * Portions Copyright 1999 John C. Quillan.
 * Portions Copyright 2002 myinternet Limited.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */

#include "perl_back.h"

int
perl_back_modify(
	Operation	*op,
	SlapReply	*rs )
{
	PerlBackend *perl_back = (PerlBackend *)op->o_bd->be_private;
	Modifications *modlist = op->orm_modlist;
	int count;
	int i;


	ldap_pvt_thread_mutex_lock( &perl_interpreter_mutex );	

	{
		dSP; ENTER; SAVETMPS;
		
		PUSHMARK(sp);
		XPUSHs( perl_back->pb_obj_ref );
		XPUSHs(sv_2mortal(newSVpv( op->o_req_dn.bv_val , 0)));

		for (; modlist != NULL; modlist = modlist->sml_next ) {
			Modification *mods = &modlist->sml_mod;

			switch ( mods->sm_op & ~LDAP_MOD_BVALUES ) {
			case LDAP_MOD_ADD:
				XPUSHs(sv_2mortal(newSVpv("ADD", 0 )));
				break;
				
			case LDAP_MOD_DELETE:
				XPUSHs(sv_2mortal(newSVpv("DELETE", 0 )));
				break;
				
			case LDAP_MOD_REPLACE:
				XPUSHs(sv_2mortal(newSVpv("REPLACE", 0 )));
				break;
			}

			
			XPUSHs(sv_2mortal(newSVpv( mods->sm_desc->ad_cname.bv_val, 0 )));

			for ( i = 0;
				mods->sm_values != NULL && mods->sm_values[i].bv_val != NULL;
				i++ )
			{
				XPUSHs(sv_2mortal(newSVpv( mods->sm_values[i].bv_val, 0 )));
			}
		}

		PUTBACK;

#ifdef PERL_IS_5_6
		count = call_method("modify", G_SCALAR);
#else
		count = perl_call_method("modify", G_SCALAR);
#endif

		SPAGAIN;

		if (count != 1) {
			croak("Big trouble in back_modify\n");
		}
							 
		rs->sr_err = POPi;

		PUTBACK; FREETMPS; LEAVE;
	}

	ldap_pvt_thread_mutex_unlock( &perl_interpreter_mutex );

	send_ldap_result( op, rs );

	Debug( LDAP_DEBUG_ANY, "Perl MODIFY\n", 0, 0, 0 );
	return( 0 );
}

