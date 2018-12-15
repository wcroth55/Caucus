/* $OpenLDAP: pkg/ldap/servers/slapd/overlays/ppolicy.c,v 1.31.2.13 2005/09/28 00:30:35 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 2004-2005 The OpenLDAP Foundation.
 * Portions Copyright 2004-2005 Howard Chu, Symas Corporation.
 * Portions Copyright 2004 Hewlett-Packard Company.
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
 * This work was developed by Howard Chu for inclusion in
 * OpenLDAP Software, based on prior work by Neil Dunbar (HP).
 * This work was sponsored by the Hewlett-Packard Company.
 */

#include "portable.h"

/* This file implements "Password Policy for LDAP Directories",
 * based on draft behera-ldap-password-policy-09
 */

#ifdef SLAPD_OVER_PPOLICY

#include <ldap.h>
#include "lutil.h"
#include "slap.h"
#if SLAPD_MODULES
#define LIBLTDL_DLL_IMPORT	/* Win32: don't re-export libltdl's symbols */
#include <ltdl.h>
#endif
#include <ac/errno.h>
#include <ac/time.h>
#include <ac/string.h>
#include <ac/ctype.h>

#ifndef MODULE_NAME_SZ
#define MODULE_NAME_SZ 256
#endif

/* Per-instance configuration information */
typedef struct pp_info {
	struct berval def_policy;	/* DN of default policy subentry */
	int use_lockout;		/* send AccountLocked result? */
	int hash_passwords;		/* transparently hash cleartext pwds */
} pp_info;

/* Our per-connection info - note, it is not per-instance, it is 
 * used by all instances
 */
typedef struct pw_conn {
	int restricted;		/* TRUE if connection is restricted */
} pw_conn;

static pw_conn *pwcons;
static int ppolicy_cid;

typedef struct pass_policy {
	AttributeDescription *ad; /* attribute to which the policy applies */
	int pwdMinAge; /* minimum time (seconds) until passwd can change */
	int pwdMaxAge; /* time in seconds until pwd will expire after change */
	int pwdInHistory; /* number of previous passwords kept */
	int pwdCheckQuality; /* 0 = don't check quality, 1 = check if possible,
						   2 = check mandatory; fail if not possible */
	int pwdMinLength; /* minimum number of chars in password */
	int pwdExpireWarning; /* number of seconds that warning controls are
							sent before a password expires */
	int pwdGraceAuthNLimit; /* number of times you can log in with an
							expired password */
	int pwdLockout; /* 0 = do not lockout passwords, 1 = lock them out */
	int pwdLockoutDuration; /* time in seconds a password is locked out for */
	int pwdMaxFailure; /* number of failed binds allowed before lockout */
	int pwdFailureCountInterval; /* number of seconds before failure
									counts are zeroed */
	int pwdMustChange; /* 0 = users can use admin set password
							1 = users must change password after admin set */
	int pwdAllowUserChange; /* 0 = users cannot change their passwords
								1 = users can change them */
	int pwdSafeModify; /* 0 = old password doesn't need to come
								with password change request
							1 = password change must supply existing pwd */
	char pwdCheckModule[MODULE_NAME_SZ]; /* name of module to dynamically
										    load to check password */
} PassPolicy;

typedef struct pw_hist {
	time_t t;	/* timestamp of history entry */
	struct berval pw;	/* old password hash */
	struct berval bv;	/* text of entire entry */
	struct pw_hist *next;
} pw_hist;

/* Operational attributes */
static AttributeDescription *ad_pwdChangedTime, *ad_pwdAccountLockedTime,
	*ad_pwdFailureTime, *ad_pwdHistory, *ad_pwdGraceUseTime, *ad_pwdReset,
	*ad_pwdPolicySubentry;

static struct schema_info {
	char *def;
	AttributeDescription **ad;
} pwd_OpSchema[] = {
	{	"( 1.3.6.1.4.1.42.2.27.8.1.16 "
		"NAME ( 'pwdChangedTime' ) "
		"DESC 'The time the password was last changed' "
		"EQUALITY generalizedTimeMatch "
		"ORDERING generalizedTimeOrderingMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.24 "
		"SINGLE-VALUE NO-USER-MODIFICATION USAGE directoryOperation )",
		&ad_pwdChangedTime },
	{	"( 1.3.6.1.4.1.42.2.27.8.1.17 "
		"NAME ( 'pwdAccountLockedTime' ) "
		"DESC 'The time an user account was locked' "
		"EQUALITY generalizedTimeMatch "
		"ORDERING generalizedTimeOrderingMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.24 "
		"SINGLE-VALUE "
#if 0
		/* Not until MANAGEDIT control is released */
		"NO-USER-MODIFICATION "
#endif
		"USAGE directoryOperation )",
		&ad_pwdAccountLockedTime },
	{	"( 1.3.6.1.4.1.42.2.27.8.1.19 "
		"NAME ( 'pwdFailureTime' ) "
		"DESC 'The timestamps of the last consecutive authentication failures' "
		"EQUALITY generalizedTimeMatch "
		"ORDERING generalizedTimeOrderingMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.24 "
		"NO-USER-MODIFICATION USAGE directoryOperation )",
		&ad_pwdFailureTime },
	{	"( 1.3.6.1.4.1.42.2.27.8.1.20 "
		"NAME ( 'pwdHistory' ) "
		"DESC 'The history of users passwords' "
		"EQUALITY octetStringMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.40 "
		"NO-USER-MODIFICATION USAGE directoryOperation )",
		&ad_pwdHistory },
	{	"( 1.3.6.1.4.1.42.2.27.8.1.21 "
		"NAME ( 'pwdGraceUseTime' ) "
		"DESC 'The timestamps of the grace login once the password has expired' "
		"EQUALITY generalizedTimeMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.24 "
		"NO-USER-MODIFICATION USAGE directoryOperation )",
		&ad_pwdGraceUseTime }, 
	{	"( 1.3.6.1.4.1.42.2.27.8.1.22 "
		"NAME ( 'pwdReset' ) "
		"DESC 'The indication that the password has been reset' "
		"EQUALITY booleanMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.7 "
		"SINGLE-VALUE USAGE directoryOperation )",
		&ad_pwdReset },
	{	"( 1.3.6.1.4.1.42.2.27.8.1.23 "
		"NAME ( 'pwdPolicySubentry' ) "
		"DESC 'The pwdPolicy subentry in effect for this object' "
		"EQUALITY distinguishedNameMatch "
		"SYNTAX 1.3.6.1.4.1.1466.115.121.1.12 "
		"SINGLE-VALUE "
#if 0
		/* Not until MANAGEDIT control is released */
		"NO-USER-MODIFICATION "
#endif
		"USAGE directoryOperation )",
		&ad_pwdPolicySubentry },
	{ NULL, NULL }
};

/* User attributes */
static AttributeDescription *ad_pwdMinAge, *ad_pwdMaxAge, *ad_pwdInHistory,
	*ad_pwdCheckQuality, *ad_pwdMinLength, *ad_pwdMaxFailure, 
	*ad_pwdGraceAuthNLimit, *ad_pwdExpireWarning, *ad_pwdLockoutDuration,
	*ad_pwdFailureCountInterval, *ad_pwdCheckModule, *ad_pwdLockout,
	*ad_pwdMustChange, *ad_pwdAllowUserChange, *ad_pwdSafeModify,
	*ad_pwdAttribute;

#define TAB(name)	{ #name, &ad_##name }

static struct schema_info pwd_UsSchema[] = {
	TAB(pwdAttribute),
	TAB(pwdMinAge),
	TAB(pwdMaxAge),
	TAB(pwdInHistory),
	TAB(pwdCheckQuality),
	TAB(pwdMinLength),
	TAB(pwdMaxFailure),
	TAB(pwdGraceAuthNLimit),
	TAB(pwdExpireWarning),
	TAB(pwdLockout),
	TAB(pwdLockoutDuration),
	TAB(pwdFailureCountInterval),
	TAB(pwdCheckModule),
	TAB(pwdMustChange),
	TAB(pwdAllowUserChange),
	TAB(pwdSafeModify),
	{ NULL, NULL }
};

static ldap_pvt_thread_mutex_t chk_syntax_mutex;

static time_t
parse_time( char *atm )
{
	struct lutil_tm tm;
	struct lutil_timet tt;
	time_t ret = (time_t)-1;

	if ( lutil_parsetime( atm, &tm ) == 0) {
		lutil_tm2time( &tm, &tt );
		ret = tt.tt_sec;
	}
	return ret;
}

static int
account_locked( Operation *op, Entry *e,
		PassPolicy *pp, Modifications **mod ) 
{
	Attribute       *la;

	assert(mod != NULL);

	if ( (la = attr_find( e->e_attrs, ad_pwdAccountLockedTime )) != NULL ) {
		BerVarray vals = la->a_nvals;

		/*
		 * there is a lockout stamp - we now need to know if it's
		 * a valid one.
		 */
		if (vals[0].bv_val != NULL) {
			time_t then, now;
			Modifications *m;

			if (!pp->pwdLockoutDuration)
				return 1;

			if ((then = parse_time( vals[0].bv_val )) == (time_t)0)
				return 1;

			now = slap_get_time();

			if (now < then + pp->pwdLockoutDuration)
				return 1;

			m = ch_calloc( sizeof(Modifications), 1 );
			m->sml_op = LDAP_MOD_DELETE;
			m->sml_flags = 0;
			m->sml_type = ad_pwdAccountLockedTime->ad_cname;
			m->sml_desc = ad_pwdAccountLockedTime;
			m->sml_next = *mod;
			*mod = m;
		}
	}

	return 0;
}

#define PPOLICY_WARNING 0xa0L
#define PPOLICY_ERROR 0xa1L
 
#define PPOLICY_EXPIRE 0xa0L
#define PPOLICY_GRACE  0xa1L

static LDAPControl *
create_passcontrol( int exptime, int grace, LDAPPasswordPolicyError err )
{
	char berbuf[LBER_ELEMENT_SIZEOF], bb2[LBER_ELEMENT_SIZEOF];
	BerElement *ber = (BerElement *)berbuf, *b2 = (BerElement *)bb2;
	LDAPControl *c;
	struct berval bv;

	if ((c = ch_calloc( sizeof( LDAPControl ), 1 )) == NULL) return NULL;
	c->ldctl_oid = LDAP_CONTROL_PASSWORDPOLICYRESPONSE;
	c->ldctl_iscritical = 0;
	c->ldctl_value.bv_val = NULL;
	c->ldctl_value.bv_len = 0;

	ber_init2( ber, NULL, LBER_USE_DER );
	ber_printf(ber, "{" /*}*/ );

	if (exptime >= 0) {
		ber_init2( b2, NULL, LBER_USE_DER );
		ber_printf( b2, "ti", PPOLICY_EXPIRE, exptime );
		ber_flatten2( b2, &bv, 1 );
		ber_printf( ber, "tO", PPOLICY_WARNING, &bv );
		ch_free( bv.bv_val );
	} else if (grace > 0) {
		ber_init2( b2, NULL, LBER_USE_DER );
		ber_printf( b2, "ti", PPOLICY_GRACE, grace );
		ber_flatten2( b2, &bv, 1 );
		ber_printf( ber, "tO", PPOLICY_WARNING, &bv );
		ch_free( bv.bv_val );
	}

	if (err != PP_noError ) {
		ber_printf( ber, "te", PPOLICY_ERROR, err );
	}
	ber_printf( ber, /*{*/ "N}" );

	if (ber_flatten2( ber, &(c->ldctl_value), 1 ) == LBER_DEFAULT) {
		ch_free(c);
		(void)ber_free_buf(ber);
		return NULL;
	}
	(void)ber_free_buf(ber);
	return c;
}

static void
ppolicy_get( Operation *op, Entry *e, PassPolicy *pp )
{
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;
	pp_info *pi = on->on_bi.bi_private;
	Attribute *a;
	BerVarray vals;
	int rc;
	Entry *pe = NULL;
#if 0
	const char *text;
#endif

	memset( pp, 0, sizeof(PassPolicy) );

	/* Users can change their own password by default */
    	pp->pwdAllowUserChange = 1;

	if ((a = attr_find( e->e_attrs, ad_pwdPolicySubentry )) == NULL) {
		/*
		 * entry has no password policy assigned - use default
		 */
		vals = &pi->def_policy;
		if ( !vals->bv_val )
			goto defaultpol;
	} else {
		vals = a->a_nvals;
		if (vals[0].bv_val == NULL) {
			Debug( LDAP_DEBUG_ANY,
				"ppolicy_get: NULL value for policySubEntry\n", 0, 0, 0 );
			goto defaultpol;
		}
	}

	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	rc = be_entry_get_rw( op, vals, NULL, NULL, 0, &pe );
	op->o_bd->bd_info = (BackendInfo *)on;

	if ( rc ) goto defaultpol;

#if 0	/* Only worry about userPassword for now */
	if ((a = attr_find( pe->e_attrs, ad_pwdAttribute )))
		slap_bv2ad( &a->a_vals[0], &pp->ad, &text );
#else
	pp->ad = slap_schema.si_ad_userPassword;
#endif

	if ((a = attr_find( pe->e_attrs, ad_pwdMinAge )))
		pp->pwdMinAge = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdMaxAge )))
		pp->pwdMaxAge = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdInHistory )))
		pp->pwdInHistory = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdCheckQuality )))
		pp->pwdCheckQuality = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdMinLength )))
		pp->pwdMinLength = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdMaxFailure )))
		pp->pwdMaxFailure = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdGraceAuthNLimit )))
		pp->pwdGraceAuthNLimit = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdExpireWarning )))
		pp->pwdExpireWarning = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdFailureCountInterval )))
		pp->pwdFailureCountInterval = atoi(a->a_vals[0].bv_val );
	if ((a = attr_find( pe->e_attrs, ad_pwdLockoutDuration )))
		pp->pwdLockoutDuration = atoi(a->a_vals[0].bv_val );

	if ((a = attr_find( pe->e_attrs, ad_pwdCheckModule ))) {
		strncpy(pp->pwdCheckModule, a->a_vals[0].bv_val,
			sizeof(pp->pwdCheckModule));
		pp->pwdCheckModule[sizeof(pp->pwdCheckModule)-1] = '\0';
	}

	if ((a = attr_find( pe->e_attrs, ad_pwdLockout )))
    	pp->pwdLockout = !strcmp( a->a_nvals[0].bv_val, "TRUE" );
	if ((a = attr_find( pe->e_attrs, ad_pwdMustChange )))
    	pp->pwdMustChange = !strcmp( a->a_nvals[0].bv_val, "TRUE" );
	if ((a = attr_find( pe->e_attrs, ad_pwdAllowUserChange )))
    	pp->pwdAllowUserChange = !strcmp( a->a_nvals[0].bv_val, "TRUE" );
	if ((a = attr_find( pe->e_attrs, ad_pwdSafeModify )))
    	pp->pwdSafeModify = !strcmp( a->a_nvals[0].bv_val, "TRUE" );
    
	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	be_entry_release_r( op, pe );
	op->o_bd->bd_info = (BackendInfo *)on;

	return;

defaultpol:
	Debug( LDAP_DEBUG_ANY,
		"ppolicy_get: using default policy\n", 0, 0, 0 );
	return;
}

static int
password_scheme( struct berval *cred, struct berval *sch )
{
	int e;
    
	assert( cred != NULL );

	if (sch) {
		sch->bv_val = NULL;
		sch->bv_len = 0;
	}
    
	if ((cred->bv_len == 0) || (cred->bv_val == NULL) ||
		(cred->bv_val[0] != '{')) return LDAP_OTHER;

	for(e = 1; cred->bv_val[e] && cred->bv_val[e] != '}'; e++);
	if (cred->bv_val[e]) {
		int rc;
		rc = lutil_passwd_scheme( cred->bv_val );
		if (rc && sch) {
			sch->bv_val = cred->bv_val;
			sch->bv_len = e;
			return LDAP_SUCCESS;
		}
	}
	return LDAP_OTHER;
}

static int
check_password_quality( struct berval *cred, PassPolicy *pp, LDAPPasswordPolicyError *err, Entry *e )
{
	int rc = LDAP_SUCCESS, ok = LDAP_SUCCESS;
	char *ptr = cred->bv_val;
	struct berval sch;

	assert( cred != NULL );
	assert( pp != NULL );

	if ((cred->bv_len == 0) || (pp->pwdMinLength > cred->bv_len)) {
		rc = LDAP_CONSTRAINT_VIOLATION;
		if ( err ) *err = PP_passwordTooShort;
		return rc;
	}

        /*
         * We need to know if the password is already hashed - if so
         * what scheme is it. The reason being that the "hash" of
         * {cleartext} still allows us to check the password.
         */
	rc = password_scheme( cred, &sch );
	if (rc == LDAP_SUCCESS) {
		if ((sch.bv_val) && (strncasecmp( sch.bv_val, "{cleartext}",
			sch.bv_len ) == 0)) {
			/*
			 * We can check the cleartext "hash"
			 */
			ptr = cred->bv_val + sch.bv_len;
		} else {
			/* everything else, we can't check */
			if (pp->pwdCheckQuality == 2) {
				rc = LDAP_CONSTRAINT_VIOLATION;
				if (err) *err = PP_insufficientPasswordQuality;
				return rc;
			}
			/*
			 * We can't check the syntax of the password, but it's not
			 * mandatory (according to the policy), so we return success.
			 */
		    
			return LDAP_SUCCESS;
		}
	}

	rc = LDAP_SUCCESS;

	if (pp->pwdCheckModule[0]) {
#if SLAPD_MODULES
		lt_dlhandle mod;
		const char *err;
		
		if ((mod = lt_dlopen( pp->pwdCheckModule )) == NULL) {
			err = lt_dlerror();

			Debug(LDAP_DEBUG_ANY,
			"check_password_quality: lt_dlopen failed: (%s) %s.\n",
				pp->pwdCheckModule, err, 0 );
			ok = LDAP_OTHER; /* internal error */
		} else {
			int (*prog)( char *passwd, char **text, Entry *ent );

			if ((prog = lt_dlsym( mod, "check_password" )) == NULL) {
				err = lt_dlerror();
			    
				Debug(LDAP_DEBUG_ANY,
					"check_password_quality: lt_dlsym failed: (%s) %s.\n",
					pp->pwdCheckModule, err, 0 );
				ok = LDAP_OTHER;
			} else {
				char *txt = NULL;

				ldap_pvt_thread_mutex_lock( &chk_syntax_mutex );
				ok = prog( cred->bv_val, &txt, e );
				ldap_pvt_thread_mutex_unlock( &chk_syntax_mutex );
				if (txt) {
					Debug(LDAP_DEBUG_ANY,
						"check_password_quality: module error: (%s) %s.[%d]\n",
						pp->pwdCheckModule, txt, ok );
					free(txt);
				} else
					ok = LDAP_SUCCESS;
			}
			    
			lt_dlclose( mod );
		}
#else
	Debug(LDAP_DEBUG_ANY, "check_password_quality: external modules not "
		"supported. pwdCheckModule ignored.\n", 0, 0, 0);
#endif /* SLAPD_MODULES */
	}
		
		    
	if (ok != LDAP_SUCCESS) {
		rc = LDAP_CONSTRAINT_VIOLATION;
		if (err) *err = PP_insufficientPasswordQuality;
	}
	
	return rc;
}

static int
parse_pwdhistory( struct berval *bv, char **oid, time_t *oldtime, struct berval *oldpw )
{
	char *ptr;
	struct berval nv, npw;
	int i, j;
	
	assert (bv && (bv->bv_len > 0) && (bv->bv_val) && oldtime && oldpw );

	if ( oid ) *oid = 0;
	*oldtime = (time_t)-1;
	oldpw->bv_val = NULL;
	oldpw->bv_len = 0;
	
	ber_dupbv( &nv, bv );

	/* first get the time field */
	for(i=0; (i < nv.bv_len) && (nv.bv_val[i] != '#'); i++);
	if ( i == nv.bv_len) goto exit_failure; /* couldn't locate the '#' separator */
	nv.bv_val[i++] = '\0'; /* terminate the string & move to next field */
	ptr = nv.bv_val;
	*oldtime = parse_time( ptr );
	if (*oldtime == (time_t)-1) goto exit_failure;

	/* get the OID field */
	for(ptr = &(nv.bv_val[i]);(i < nv.bv_len) && (nv.bv_val[i] != '#'); i++);
	if ( i == nv.bv_len) goto exit_failure; /* couldn't locate the '#' separator */
	nv.bv_val[i++] = '\0'; /* terminate the string & move to next field */
	if ( oid ) *oid = ber_strdup( ptr );
	
	/* get the length field */
	for(ptr = &(nv.bv_val[i]);(i < nv.bv_len) && (nv.bv_val[i] != '#'); i++);
	if ( i == nv.bv_len) goto exit_failure; /* couldn't locate the '#' separator */
	nv.bv_val[i++] = '\0'; /* terminate the string & move to next field */
	oldpw->bv_len = strtol( ptr, NULL, 10 );
	if (errno == ERANGE) goto exit_failure;

	/* lastly, get the octets of the string */
	for(j=i, ptr = &(nv.bv_val[i]);i < nv.bv_len; i++);
	if (i-j != oldpw->bv_len) goto exit_failure; /* length is wrong */

	npw.bv_val = ptr;
	npw.bv_len = oldpw->bv_len;
	ber_dupbv( oldpw, &npw );
	
	return LDAP_SUCCESS;
exit_failure:
	if (oid && *oid) { ber_memfree(*oid); *oid = NULL; }
	if (oldpw->bv_val) {
		ber_memfree( oldpw->bv_val); oldpw->bv_val = NULL;
		oldpw->bv_len = 0;
	}
	ber_memfree(nv.bv_val);
	return LDAP_OTHER;
}

static void
add_to_pwd_history( pw_hist **l, time_t t,
                    struct berval *oldpw, struct berval *bv )
{
	pw_hist *p, *p1, *p2;
    
	if (!l) return;

	p = ch_malloc( sizeof( pw_hist ));
	p->pw = *oldpw;
	ber_dupbv( &p->bv, bv );
	p->t = t;
	p->next = NULL;
	
	if (*l == NULL) {
		/* degenerate case */
		*l = p;
		return;
	}
	/*
	 * advance p1 and p2 such that p1 is the node before the
	 * new one, and p2 is the node after it
	 */
	for (p1 = NULL, p2 = *l; p2 && p2->t <= t; p1 = p2, p2=p2->next );
	p->next = p2;
	if (p1 == NULL) { *l = p; return; }
	p1->next = p;
}

#ifndef MAX_PWD_HISTORY_SZ
#define MAX_PWD_HISTORY_SZ 1024
#endif /* MAX_PWD_HISTORY_SZ */

static void
make_pwd_history_value( char *timebuf, struct berval *bv, Attribute *pa )
{
	char str[ MAX_PWD_HISTORY_SZ ];
	int nlen;

	snprintf( str, MAX_PWD_HISTORY_SZ,
		  "%s#%s#%lu#", timebuf,
		  pa->a_desc->ad_type->sat_syntax->ssyn_oid,
		  (unsigned long) pa->a_nvals[0].bv_len );
	str[MAX_PWD_HISTORY_SZ-1] = 0;
	nlen = strlen(str);

        /*
         * We have to assume that the string is a string of octets,
         * not readable characters. In reality, yes, it probably is
         * a readable (ie, base64) string, but we can't count on that
         * Hence, while the first 3 fields of the password history
         * are definitely readable (a timestamp, an OID and an integer
         * length), the remaining octets of the actual password
         * are deemed to be binary data.
         */
	AC_MEMCPY( str + nlen, pa->a_nvals[0].bv_val, pa->a_nvals[0].bv_len );
	nlen += pa->a_nvals[0].bv_len;
	bv->bv_val = ch_malloc( nlen + 1 );
	AC_MEMCPY( bv->bv_val, str, nlen );
	bv->bv_val[nlen] = '\0';
	bv->bv_len = nlen;
}

static void
free_pwd_history_list( pw_hist **l )
{
	pw_hist *p;
    
	if (!l) return;
	p = *l;
	while (p) {
		pw_hist *pp = p->next;

		free(p->pw.bv_val);
		free(p->bv.bv_val);
		free(p);
		p = pp;
	}
	*l = NULL;
}

typedef struct ppbind {
	slap_overinst *on;
	int send_ctrl;
	Modifications *mod;
	LDAPPasswordPolicyError pErr;
	PassPolicy pp;
} ppbind;

static int
ppolicy_bind_resp( Operation *op, SlapReply *rs )
{
	ppbind *ppb = op->o_callback->sc_private;
	slap_overinst *on = ppb->on;
	Modifications *mod = ppb->mod, *m;
	int pwExpired = 0;
	int ngut = -1, warn = -1, age, rc;
	Attribute *a;
	time_t now, pwtime = (time_t)-1;
	char nowstr[ LDAP_LUTIL_GENTIME_BUFSIZE ];
	struct berval timestamp;
	BackendInfo *bi = op->o_bd->bd_info;
	Entry *e;

	/* If we already know it's locked, just get on with it */
	if ( ppb->pErr != PP_noError ) {
		goto locked;
	}

	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	rc = be_entry_get_rw( op, &op->o_req_ndn, NULL, NULL, 0, &e );
	op->o_bd->bd_info = bi;

	if ( rc != LDAP_SUCCESS ) {
		return SLAP_CB_CONTINUE;
	}

	now = slap_get_time(); /* stored for later consideration */
	timestamp.bv_val = nowstr;
	timestamp.bv_len = sizeof(nowstr);
	slap_timestamp( &now, &timestamp );

	if ( rs->sr_err == LDAP_INVALID_CREDENTIALS ) {
		int i = 0, fc = 0;

		m = ch_calloc( sizeof(Modifications), 1 );
		m->sml_op = LDAP_MOD_ADD;
		m->sml_flags = 0;
		m->sml_type = ad_pwdFailureTime->ad_cname;
		m->sml_desc = ad_pwdFailureTime;
		m->sml_values = ch_calloc( sizeof(struct berval), 2 );

		ber_dupbv( &m->sml_values[0], &timestamp );
		m->sml_next = mod;
		mod = m;

		/*
		 * Count the pwdFailureTimes - if it's
		 * greater than the policy pwdMaxFailure,
		 * then lock the account.
		 */
		if ((a = attr_find( e->e_attrs, ad_pwdFailureTime )) != NULL) {
			for(i=0; a->a_nvals[i].bv_val; i++) {

				/*
				 * If the interval is 0, then failures
				 * stay on the record until explicitly
				 * reset by successful authentication.
				 */
				if (ppb->pp.pwdFailureCountInterval == 0) {
					fc++;
				} else if (now <=
							parse_time(a->a_nvals[i].bv_val) +
							ppb->pp.pwdFailureCountInterval) {

					fc++;
				}
				/*
				 * We only count those failures
				 * which are not due to expire.
				 */
			}
		}
		
		if ((ppb->pp.pwdMaxFailure > 0) &&
			(fc >= ppb->pp.pwdMaxFailure - 1)) {

			/*
			 * We subtract 1 from the failure max
			 * because the new failure entry hasn't
			 * made it to the entry yet.
			 */
			m = ch_calloc( sizeof(Modifications), 1 );
			m->sml_op = LDAP_MOD_REPLACE;
			m->sml_flags = 0;
			m->sml_type = ad_pwdAccountLockedTime->ad_cname;
			m->sml_desc = ad_pwdAccountLockedTime;
			m->sml_values = ch_calloc( sizeof(struct berval), 2 );
			ber_dupbv( &m->sml_values[0], &timestamp );
			m->sml_next = mod;
			mod = m;
		}
	} else if ( rs->sr_err == LDAP_SUCCESS ) {
		if ((a = attr_find( e->e_attrs, ad_pwdChangedTime )) != NULL)
			pwtime = parse_time( a->a_nvals[0].bv_val );

		/* delete all pwdFailureTimes */
		if ( attr_find( e->e_attrs, ad_pwdFailureTime )) {
			m = ch_calloc( sizeof(Modifications), 1 );
			m->sml_op = LDAP_MOD_DELETE;
			m->sml_flags = 0;
			m->sml_type = ad_pwdFailureTime->ad_cname;
			m->sml_desc = ad_pwdFailureTime;
			m->sml_next = mod;
			mod = m;
		}

		/*
		 * check to see if the password must be changed
		 */
		if ( ppb->pp.pwdMustChange &&
			(a = attr_find( e->e_attrs, ad_pwdReset )) &&
			!strcmp( a->a_nvals[0].bv_val, "TRUE" ) ) {
			/*
			 * need to inject client controls here to give
			 * more information. For the moment, we ensure
			 * that we are disallowed from doing anything
			 * other than change password.
			 */
			pwcons[op->o_conn->c_conn_idx].restricted = 1;

			ppb->pErr = PP_changeAfterReset;

		} else {
			/*
			 * the password does not need to be changed, so
			 * we now check whether the password has expired.
			 *
			 * We can skip this bit if passwords don't age in
			 * the policy.
			 */
			if (ppb->pp.pwdMaxAge == 0) goto grace;

			if (pwtime == (time_t)-1) {
				/*
				 * Hmm. No password changed time on the
				 * entry. This is odd - it should have
				 * been provided when the attribute was added.
				 *
				 * However, it's possible that it could be
				 * missing if the DIT was established via
				 * an import process.
				 */
				Debug( LDAP_DEBUG_ANY,
					"ppolicy_bind: Entry %s does not have valid pwdChangedTime attribute - assuming password expired\n",
					e->e_name.bv_val, 0, 0);
				
				pwExpired = 1;
			} else {
				/*
				 * Check: was the last change time of
				 * the password older than the maximum age
				 * allowed. (Ignore case 2 from I-D, it's just silly.)
				 */
				if (now - pwtime > ppb->pp.pwdMaxAge ) pwExpired = 1;
			}
		}

grace:
		if (!pwExpired) goto check_expiring_password;
		
		if ((a = attr_find( e->e_attrs, ad_pwdGraceUseTime )) == NULL)
			ngut = ppb->pp.pwdGraceAuthNLimit;
		else {
			for(ngut=0; a->a_nvals[ngut].bv_val; ngut++);
			ngut = ppb->pp.pwdGraceAuthNLimit - ngut;
		}

		/*
		 * ngut is the number of remaining grace logins
		 */
		Debug( LDAP_DEBUG_ANY,
			"ppolicy_bind: Entry %s has an expired password: %d grace logins\n",
			e->e_name.bv_val, ngut, 0);
		
		if (ngut < 1) {
			ppb->pErr = PP_passwordExpired;
			rs->sr_err = LDAP_INVALID_CREDENTIALS;
			goto done;
		}

		/*
		 * Add a grace user time to the entry
		 */
		m = ch_calloc( sizeof(Modifications), 1 );
		m->sml_op = LDAP_MOD_ADD;
		m->sml_flags = 0;
		m->sml_type = ad_pwdGraceUseTime->ad_cname;
		m->sml_desc = ad_pwdGraceUseTime;
		m->sml_values = ch_calloc( sizeof(struct berval), 2 );
		ber_dupbv( &m->sml_values[0], &timestamp );
		m->sml_next = mod;
		mod = m;

check_expiring_password:
		/*
		 * Now we need to check to see
		 * if it is about to expire, and if so, should the user
		 * be warned about it in the password policy control.
		 *
		 * If the password has expired, and we're in the grace period, then
		 * we don't need to do this bit. Similarly, if we don't have password
		 * aging, then there's no need to do this bit either.
		 */
		if ((ppb->pp.pwdMaxAge < 1) || (pwExpired) || (ppb->pp.pwdExpireWarning < 1))
			goto done;

		age = (int)(now - pwtime);
		
		/*
		 * We know that there is a password Change Time attribute - if
		 * there wasn't, then the pwdExpired value would be true, unless
		 * there is no password aging - and if there is no password aging,
		 * then this section isn't called anyway - you can't have an
		 * expiring password if there's no limit to expire.
		 */
		if (ppb->pp.pwdMaxAge - age < ppb->pp.pwdExpireWarning ) {
			/*
			 * Set the warning value.
			 */
			warn = ppb->pp.pwdMaxAge - age; /* seconds left until expiry */
			if (warn < 0) warn = 0; /* something weird here - why is pwExpired not set? */
			
			Debug( LDAP_DEBUG_ANY,
				"ppolicy_bind: Setting warning for password expiry for %s = %d seconds\n",
				op->o_req_dn.bv_val, warn, 0 );
		}
	}

done:
	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	be_entry_release_r( op, e );

locked:
	if ( mod ) {
		Operation op2 = *op;
		SlapReply r2 = { REP_RESULT };
		slap_callback cb = { NULL, slap_null_cb, NULL, NULL };

		/* FIXME: Need to handle replication of some (but not all)
		 * of the operational attributes...
		 */
		op2.o_tag = LDAP_REQ_MODIFY;
		op2.o_callback = &cb;
		op2.orm_modlist = mod;
		op2.o_dn = op->o_bd->be_rootdn;
		op2.o_ndn = op->o_bd->be_rootndn;
		op2.o_bd->bd_info = (BackendInfo *)on->on_info;
		rc = op->o_bd->be_modify( &op2, &r2 );
		slap_mods_free( mod, 1 );
	}

	if ( ppb->send_ctrl ) {
		LDAPControl **ctrls = NULL;
		pp_info *pi = on->on_bi.bi_private;

		/* Do we really want to tell that the account is locked? */
		if ( ppb->pErr == PP_accountLocked && !pi->use_lockout ) {
			ppb->pErr = PP_noError;
		}
		ctrls = ch_calloc( sizeof( LDAPControl *) , 2 );
		ctrls[0] = create_passcontrol( warn, ngut, ppb->pErr );
		ctrls[1] = NULL;
		rs->sr_ctrls = ctrls;
	}
	op->o_bd->bd_info = bi;
	return SLAP_CB_CONTINUE;
}

static int
ppolicy_bind( Operation *op, SlapReply *rs )
{
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;

	/* Reset lockout status on all Bind requests */
	pwcons[op->o_conn->c_conn_idx].restricted = 0;

	/* Root bypasses policy */
	if ( !be_isroot_dn( op->o_bd, &op->o_req_ndn )) {
		Entry *e;
		int rc;
		ppbind *ppb;
		slap_callback *cb;

		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		rc = be_entry_get_rw( op, &op->o_req_ndn, NULL, NULL, 0, &e );

		if ( rc != LDAP_SUCCESS ) {
			return SLAP_CB_CONTINUE;
		}

		cb = op->o_tmpcalloc( sizeof(ppbind)+sizeof(slap_callback),
			1, op->o_tmpmemctx );
		ppb = (ppbind *)(cb+1);
		ppb->on = on;
		ppb->pErr = PP_noError;

		/* Setup a callback so we can munge the result */

		cb->sc_response = ppolicy_bind_resp;
		cb->sc_next = op->o_callback->sc_next;
		cb->sc_private = ppb;
		op->o_callback->sc_next = cb;

		/* Did we receive a password policy request control? */
		if ( op->o_ctrlflag[ppolicy_cid] ) {
			ppb->send_ctrl = 1;
		}

		op->o_bd->bd_info = (BackendInfo *)on;
		ppolicy_get( op, e, &ppb->pp );

		rc = account_locked( op, e, &ppb->pp, &ppb->mod );

		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		be_entry_release_r( op, e );

		if ( rc ) {
			/* This will be the Draft 8 response, Unwilling is bogus */
			ppb->pErr = PP_accountLocked;
			send_ldap_error( op, rs, LDAP_INVALID_CREDENTIALS, NULL );
			return rs->sr_err;
		}

	}

	return SLAP_CB_CONTINUE;
}

/* Reset the restricted flag for the next session on this connection */
static int
ppolicy_connection_destroy( BackendDB *bd, Connection *conn )
{
	pwcons[conn->c_conn_idx].restricted = 0;
	return SLAP_CB_CONTINUE;
}

/* Check if this connection is restricted */
static int
ppolicy_restrict(
	Operation *op,
	SlapReply *rs )
{
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;
	int send_ctrl = 0;

	/* Did we receive a password policy request control? */
	if ( op->o_ctrlflag[ppolicy_cid] ) {
		send_ctrl = 1;
	}

	if ( op->o_conn && pwcons[op->o_conn->c_conn_idx].restricted ) {
		Debug( LDAP_DEBUG_TRACE,
			"connection restricted to password changing only\n", 0, 0, 0);
		if ( send_ctrl ) {
			LDAPControl **ctrls = NULL;

			ctrls = ch_calloc( sizeof( LDAPControl *) , 2 );
			ctrls[0] = create_passcontrol( -1, -1, PP_changeAfterReset );
			ctrls[1] = NULL;
			rs->sr_ctrls = ctrls;
		}
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		send_ldap_error( op, rs, LDAP_INSUFFICIENT_ACCESS, 
			"Operations are restricted to bind/unbind/abandon/StartTLS/modify password" );
		return rs->sr_err;
	}

	return SLAP_CB_CONTINUE;
}

static int
ppolicy_add(
	Operation *op,
	SlapReply *rs )
{
	slap_overinst *on = (slap_overinst *)op->o_bd->bd_info;
	pp_info *pi = on->on_bi.bi_private;
	PassPolicy pp;
	Attribute *pa;
	const char *txt;

	if ( ppolicy_restrict( op, rs ) != SLAP_CB_CONTINUE )
		return rs->sr_err;

	/* If this is a replica, assume the master checked everything */
	if ( be_shadow_update( op ))
		return SLAP_CB_CONTINUE;

	/* Check for password in entry */
	if ((pa = attr_find( op->oq_add.rs_e->e_attrs,
		slap_schema.si_ad_userPassword )))
	{
		/*
		 * new entry contains a password - if we're not the root user
		 * then we need to check that the password fits in with the
		 * security policy for the new entry.
		 */
		ppolicy_get( op, op->ora_e, &pp );
		if (pp.pwdCheckQuality > 0 && !be_isroot( op )) {
			struct berval *bv = &(pa->a_vals[0]);
			int rc, send_ctrl = 0;
			LDAPPasswordPolicyError pErr = PP_noError;

			/* Did we receive a password policy request control? */
			if ( op->o_ctrlflag[ppolicy_cid] ) {
				send_ctrl = 1;
			}
			rc = check_password_quality( bv, &pp, &pErr, op->ora_e );
			if (rc != LDAP_SUCCESS) {
				op->o_bd->bd_info = (BackendInfo *)on->on_info;
				if ( send_ctrl ) {
					LDAPControl **ctrls = NULL;

					ctrls = ch_calloc( sizeof( LDAPControl *) , 2 );
					ctrls[0] = create_passcontrol( -1, -1, pErr );
					ctrls[1] = NULL;
					rs->sr_ctrls = ctrls;
				}
				send_ldap_error( op, rs, rc, "Password fails quality checking policy" );
				return rs->sr_err;
			}
			    /*
			     * A controversial bit. We hash cleartext
			     * passwords provided via add and modify operations
			     * You're not really supposed to do this, since
			     * the X.500 model says "store attributes" as they
			     * get provided. By default, this is what we do
			     *
			     * But if the hash_passwords flag is set, we hash
			     * any cleartext password attribute values via the
			     * default password hashing scheme.
			     */
			if ((pi->hash_passwords) &&
				(password_scheme( &(pa->a_vals[0]), NULL ) != LDAP_SUCCESS)) {
				struct berval hpw;

				slap_passwd_hash( &(pa->a_vals[0]), &hpw, &txt );
				if (hpw.bv_val == NULL) {
				    /*
				     * hashing didn't work. Emit an error.
				     */
					rs->sr_err = LDAP_OTHER;
					rs->sr_text = txt;
					send_ldap_error( op, rs, LDAP_OTHER, "Password hashing failed" );
					return rs->sr_err;
				}

				memset( pa->a_vals[0].bv_val, 0, pa->a_vals[0].bv_len);
				ber_memfree( pa->a_vals[0].bv_val );
				pa->a_vals[0].bv_val = hpw.bv_val;
				pa->a_vals[0].bv_len = hpw.bv_len;
			}
		}
		/* If password aging is in effect, set the pwdChangedTime */
		if ( pp.pwdMaxAge || pp.pwdMinAge ) {
			struct berval timestamp;
			char timebuf[ LDAP_LUTIL_GENTIME_BUFSIZE ];
			time_t now = slap_get_time();

			timestamp.bv_val = timebuf;
			timestamp.bv_len = sizeof(timebuf);
			slap_timestamp( &now, &timestamp );

			attr_merge_one( op->ora_e, ad_pwdChangedTime, &timestamp, NULL );
		}
	}
	return SLAP_CB_CONTINUE;
}

static int
ppolicy_modify( Operation *op, SlapReply *rs )
{
	slap_overinst		*on = (slap_overinst *)op->o_bd->bd_info;
	pp_info			*pi = on->on_bi.bi_private;
	int			i, rc, mod_pw_only, pwmod, pwmop, deladd,
				hsize = 0;
	PassPolicy		pp;
	Modifications		*mods = NULL, *modtail, *ml, *delmod, *addmod;
	Attribute		*pa, *ha, at;
	const char		*txt;
	pw_hist			*tl = NULL, *p;
	int			zapReset, send_ctrl = 0;
	Entry			*e;
	struct berval		newpw = BER_BVNULL, oldpw = BER_BVNULL,
				*bv, cr[2];
	LDAPPasswordPolicyError pErr = PP_noError;

	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	rc = be_entry_get_rw( op, &op->o_req_ndn, NULL, NULL, 0, &e );
	op->o_bd->bd_info = (BackendInfo *)on;

	if ( rc != LDAP_SUCCESS ) return SLAP_CB_CONTINUE;

	/* If this is a replica, we may need to tweak some of the
	 * master's modifications. Otherwise, just pass it through.
	 */
	if ( be_shadow_update( op )) {
		Modifications **prev;
		int got_del_grace = 0, got_del_lock = 0, got_pw = 0;
		Attribute *a_grace, *a_lock;

		a_grace = attr_find( e->e_attrs, ad_pwdGraceUseTime );
		a_lock = attr_find( e->e_attrs, ad_pwdAccountLockedTime );

		for( prev = &op->oq_modify.rs_modlist, ml = *prev; ml;
			prev = &ml->sml_next, ml = *prev ) {

			if ( ml->sml_desc == slap_schema.si_ad_userPassword )
				got_pw = 1;

			/* If we're deleting an attr that didn't exist,
			 * drop this delete op
			 */
			if ( ml->sml_op == LDAP_MOD_DELETE ) {
				int drop = 0;

				if ( ml->sml_desc == ad_pwdGraceUseTime ) {
					got_del_grace = 1;
					if ( !a_grace )
						drop = 1;
				} else
				if ( ml->sml_desc == ad_pwdAccountLockedTime ) {
					got_del_lock = 1;
					if ( !a_lock )
						drop = 1;
				}
				if ( drop ) {
					*prev = ml->sml_next;
					ml->sml_next = NULL;
					slap_mods_free( ml, 1 );
				}
			}
		}

		/* If we're resetting the password, make sure grace and accountlock
		 * also get removed.
		 */
		if ( got_pw ) {
			if ( a_grace && !got_del_grace ) {
				ml = (Modifications *) ch_malloc( sizeof( Modifications ) );
				ml->sml_op = LDAP_MOD_DELETE;
				ml->sml_flags = SLAP_MOD_INTERNAL;
				ml->sml_type.bv_val = NULL;
				ml->sml_desc = ad_pwdGraceUseTime;
				ml->sml_values = NULL;
				ml->sml_nvalues = NULL;
				ml->sml_next = NULL;
				*prev = ml;
				prev = &ml->sml_next;
			}
			if ( a_lock && !got_del_lock ) {
				ml = (Modifications *) ch_malloc( sizeof( Modifications ) );
				ml->sml_op = LDAP_MOD_DELETE;
				ml->sml_flags = SLAP_MOD_INTERNAL;
				ml->sml_type.bv_val = NULL;
				ml->sml_desc = ad_pwdAccountLockedTime;
				ml->sml_values = NULL;
				ml->sml_nvalues = NULL;
				ml->sml_next = NULL;
				*prev = ml;
			}
		}
		op->o_bd->bd_info = (BackendInfo *)on->on_info;
		be_entry_release_r( op, e );
		return SLAP_CB_CONTINUE;
	}

	/* Did we receive a password policy request control? */
	if ( op->o_ctrlflag[ppolicy_cid] ) {
		send_ctrl = 1;
	}

	/* See if this is a pwdModify exop. If so, we can
	 * access the plaintext passwords from that request.
	 */
	{
		slap_callback *sc;

		for ( sc = op->o_callback; sc; sc=sc->sc_next ) {
			if ( sc->sc_response == slap_replog_cb &&
				sc->sc_private ) {
				req_pwdexop_s *qpw = sc->sc_private;
				newpw = qpw->rs_new;
				oldpw = qpw->rs_old;
			   	break;
			}
		}
	}

	ppolicy_get( op, e, &pp );

	for(ml = op->oq_modify.rs_modlist,
			pwmod = 0, mod_pw_only = 1,
			deladd = 0, delmod = NULL,
			addmod = NULL,
			zapReset = 1;
		ml != NULL; modtail = ml, ml = ml->sml_next ) {
		if ( ml->sml_desc == pp.ad ) {
			pwmod = 1;
			pwmop = ml->sml_op;
			if ((deladd == 0) && (ml->sml_op == LDAP_MOD_DELETE) &&
				(ml->sml_values) && (ml->sml_values[0].bv_val != NULL)) {
				deladd = 1;
				delmod = ml;
			}

			if ((deladd == 1) && ((ml->sml_op == LDAP_MOD_ADD) ||
								  (ml->sml_op == LDAP_MOD_REPLACE)))
				deladd = 2;

			if ((ml->sml_op == LDAP_MOD_ADD) ||
				(ml->sml_op == LDAP_MOD_REPLACE))
				addmod = ml;
		} else if (! is_at_operational( ml->sml_desc->ad_type )) {
			mod_pw_only = 0;
			/* modifying something other than password */
		}

		/*
		 * If there is a request to explicitly add a pwdReset
		 * attribute, then we suppress the normal behaviour on
		 * password change, which is to remove the pwdReset
		 * attribute.
		 *
		 * This enables an administrator to assign a new password
		 * and place a "must reset" flag on the entry, which will
		 * stay until the user explicitly changes his/her password.
		 */
		if (ml->sml_desc == ad_pwdReset ) {
			if ((ml->sml_op == LDAP_MOD_ADD) ||
				(ml->sml_op == LDAP_MOD_REPLACE))
				zapReset = 0;
		}
	}
	
	if (pwcons[op->o_conn->c_conn_idx].restricted && !mod_pw_only) {
		Debug( LDAP_DEBUG_TRACE,
			"connection restricted to password changing only\n", 0, 0, 0 );
		rs->sr_err = LDAP_INSUFFICIENT_ACCESS; 
		rs->sr_text = "Operations are restricted to bind/unbind/abandon/StartTLS/modify password";
		pErr = PP_changeAfterReset;
		goto return_results;
	}

	/*
	 * if we have a "safe password modify policy", then we need to check if we're doing
	 * a delete (with the old password), followed by an add (with the new password).
	 *
	 * If we don't have this, then we fail with an error. We also skip all the checks if
	 * the root user is bound. Root can do anything, including avoid the policies.
	 */

	if (!pwmod) goto do_modify;

	/*
	 * Did we get a valid add mod?
	 */

	if (!addmod) {
		rs->sr_err = LDAP_OTHER;
		rs->sr_text = "Internal Error";
		Debug( LDAP_DEBUG_TRACE,
			"cannot locate modification supplying new password\n", 0, 0, 0 );
		goto return_results;
	}

	/*
	 * Build the password history list in ascending time order
	 * We need this, even if the user is root, in order to maintain
	 * the pwdHistory operational attributes properly.
	 */
	if (pp.pwdInHistory > 0 && (ha = attr_find( e->e_attrs, ad_pwdHistory ))) {
		struct berval oldpw;
		time_t oldtime;

		for(i=0; ha->a_nvals[i].bv_val; i++) {
			rc = parse_pwdhistory( &(ha->a_nvals[i]), NULL,
				&oldtime, &oldpw );

			if (rc != LDAP_SUCCESS) continue; /* invalid history entry */

			if (oldpw.bv_val) {
				add_to_pwd_history( &tl, oldtime, &oldpw,
					&(ha->a_nvals[i]) );
				oldpw.bv_val = NULL;
				oldpw.bv_len = 0;
			}
		}
		for(p=tl; p; p=p->next, hsize++); /* count history size */
	}

	if (be_isroot( op )) goto do_modify;

	/* This is a pwdModify exop that provided the old pw.
	 * We need to create a Delete mod for this old pw and 
	 * let the matching value get found later
	 */
	if (pp.pwdSafeModify && oldpw.bv_val ) {
		ml = (Modifications *) ch_malloc( sizeof( Modifications ) );
		ml->sml_op = LDAP_MOD_DELETE;
		ml->sml_flags = SLAP_MOD_INTERNAL;
		ml->sml_desc = pp.ad;
		ml->sml_type = pp.ad->ad_cname;
		ml->sml_values = (BerVarray) ch_malloc( 2 * sizeof( struct berval ) );
		ber_dupbv( &ml->sml_values[0], &oldpw );
		ml->sml_values[1].bv_len = 0;
		ml->sml_values[1].bv_val = NULL;
		ml->sml_nvalues = NULL;
		ml->sml_next = op->orm_modlist;
		op->orm_modlist = ml;
		delmod = ml;
		deladd = 2;
	}

	if (pp.pwdSafeModify && deladd != 2) {
		Debug( LDAP_DEBUG_TRACE,
			"change password must use DELETE followed by ADD/REPLACE\n",
			0, 0, 0 );
		rs->sr_err = LDAP_INSUFFICIENT_ACCESS;
		rs->sr_text = "Must supply old password to be changed as well as new one";
		pErr = PP_mustSupplyOldPassword;
		goto return_results;
	}

	if (!pp.pwdAllowUserChange) {
		rs->sr_err = LDAP_INSUFFICIENT_ACCESS;
		rs->sr_text = "User alteration of password is not allowed";
		pErr = PP_passwordModNotAllowed;
		goto return_results;
	}

	if (pp.pwdMinAge > 0) {
		time_t pwtime = (time_t)-1, now;
		int age;

		if ((pa = attr_find( e->e_attrs, ad_pwdChangedTime )) != NULL)
			pwtime = parse_time( pa->a_nvals[0].bv_val );
		now = slap_get_time();
		age = (int)(now - pwtime);
		if ((pwtime != (time_t)-1) && (age < pp.pwdMinAge)) {
			rs->sr_err = LDAP_CONSTRAINT_VIOLATION;
			rs->sr_text = "Password is too young to change";
			pErr = PP_passwordTooYoung;
			goto return_results;
		}
	}

	/* pa is used in password history check below, be sure it's set */
	if ((pa = attr_find( e->e_attrs, pp.ad )) != NULL && delmod) {
		/*
		 * we have a password to check
		 */
		const char *txt;
		
		bv = oldpw.bv_val ? &oldpw : delmod->sml_values;
		/* FIXME: no access checking? */
		rc = slap_passwd_check( op, NULL, pa, bv, &txt );
		if (rc != LDAP_SUCCESS) {
			Debug( LDAP_DEBUG_TRACE,
				"old password check failed: %s\n", txt, 0, 0 );
			
			rs->sr_err = LDAP_UNWILLING_TO_PERFORM;
			rs->sr_text = "Must supply correct old password to change to new one";
			pErr = PP_mustSupplyOldPassword;
			goto return_results;

		} else {
			int i;
			
			/*
			 * replace the delete value with the (possibly hashed)
			 * value which is currently in the password.
			 */
			for(i=0; delmod->sml_values[i].bv_val; i++) {
				free(delmod->sml_values[i].bv_val);
				delmod->sml_values[i].bv_len = 0;
			}
			free(delmod->sml_values);
			delmod->sml_values = ch_calloc( sizeof(struct berval), 2 );
			delmod->sml_values[1].bv_len = 0;
			delmod->sml_values[1].bv_val = NULL;
			ber_dupbv(&(delmod->sml_values[0]),  &(pa->a_nvals[0]));
		}
	}

	bv = newpw.bv_val ? &newpw : addmod->sml_values;
	if (pp.pwdCheckQuality > 0) {

		rc = check_password_quality( bv, &pp, &pErr, e );
		if (rc != LDAP_SUCCESS) {
			rs->sr_err = rc;
			rs->sr_text = "Password fails quality checking policy";
			goto return_results;
		}
	}

	if (pa) {
		/*
		 * Last check - the password history.
		 */
		/* FIXME: no access checking? */
		if (slap_passwd_check( op, NULL, pa, bv, &txt ) == LDAP_SUCCESS) {
			/*
			 * This is bad - it means that the user is attempting
			 * to set the password to the same as the old one.
			 */
			rs->sr_err = LDAP_CONSTRAINT_VIOLATION;
			rs->sr_text = "Password is not being changed from existing value";
			pErr = PP_passwordInHistory;
			goto return_results;
		}
	
		if (pp.pwdInHistory < 1) goto do_modify;
	
		/*
		 * Iterate through the password history, and fail on any
		 * password matches.
		 */
		at = *pa;
		at.a_vals = cr;
		cr[1].bv_val = NULL;
		for(p=tl; p; p=p->next) {
			cr[0] = p->pw;
			/* FIXME: no access checking? */
			rc = slap_passwd_check( op, NULL, &at, bv, &txt );
			
			if (rc != LDAP_SUCCESS) continue;
			
			rs->sr_err = LDAP_CONSTRAINT_VIOLATION;
			rs->sr_text = "Password is in history of old passwords";
			pErr = PP_passwordInHistory;
			goto return_results;
		}
	}

do_modify:
	if (pwmod) {
		struct berval timestamp;
		char timebuf[ LDAP_LUTIL_GENTIME_BUFSIZE ];
		time_t now = slap_get_time();
		
		/*
		 * keep the necessary pwd.. operational attributes
		 * up to date.
		 */

		timestamp.bv_val = timebuf;
		timestamp.bv_len = sizeof(timebuf);
		slap_timestamp( &now, &timestamp );

		mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
		mods->sml_type.bv_val = NULL;
		mods->sml_desc = ad_pwdChangedTime;
		if (pwmop != LDAP_MOD_DELETE) {
			mods->sml_op = LDAP_MOD_REPLACE;
			mods->sml_values = (BerVarray) ch_malloc( 2 * sizeof( struct berval ) );
			ber_dupbv( &mods->sml_values[0], &timestamp );
			mods->sml_values[1].bv_len = 0;
			mods->sml_values[1].bv_val = NULL;
			assert( mods->sml_values[0].bv_val != NULL );
		} else {
			mods->sml_op = LDAP_MOD_DELETE;
			mods->sml_values = NULL;
		}
		mods->sml_flags = SLAP_MOD_INTERNAL;
		mods->sml_nvalues = NULL;
		mods->sml_next = NULL;
		modtail->sml_next = mods;
		modtail = mods;

		if (attr_find(e->e_attrs, ad_pwdGraceUseTime )) {
			mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
			mods->sml_op = LDAP_MOD_DELETE;
			mods->sml_flags = SLAP_MOD_INTERNAL;
			mods->sml_type.bv_val = NULL;
			mods->sml_desc = ad_pwdGraceUseTime;
			mods->sml_values = NULL;
			mods->sml_nvalues = NULL;
			mods->sml_next = NULL;
			modtail->sml_next = mods;
			modtail = mods;
		}

		if (attr_find(e->e_attrs, ad_pwdAccountLockedTime )) {
			mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
			mods->sml_op = LDAP_MOD_DELETE;
			mods->sml_flags = SLAP_MOD_INTERNAL;
			mods->sml_type.bv_val = NULL;
			mods->sml_desc = ad_pwdAccountLockedTime;
			mods->sml_values = NULL;
			mods->sml_nvalues = NULL;
			mods->sml_next = NULL;
			modtail->sml_next = mods;
			modtail = mods;
		}

		/* Delete the pwdReset attribute, since it's being reset */
		if ((zapReset) && (attr_find(e->e_attrs, ad_pwdReset ))) {
			mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
			mods->sml_op = LDAP_MOD_DELETE;
			mods->sml_flags = SLAP_MOD_INTERNAL;
			mods->sml_type.bv_val = NULL;
			mods->sml_desc = ad_pwdReset;
			mods->sml_values = NULL;
			mods->sml_nvalues = NULL;
			mods->sml_next = NULL;
			modtail->sml_next = mods;
			modtail = mods;
		}

		if (pp.pwdInHistory > 0) {
			if (hsize >= pp.pwdInHistory) {
				/*
				 * We use the >= operator, since we are going to add
				 * the existing password attribute value into the
				 * history - thus the cardinality of history values is
				 * about to rise by one.
				 *
				 * If this would push it over the limit of history
				 * values (remembering - the password policy could have
				 * changed since the password was last altered), we must
				 * delete at least 1 value from the pwdHistory list.
				 *
				 * In fact, we delete '(#pwdHistory attrs - max pwd
				 * history length) + 1' values, starting with the oldest.
				 * This is easily evaluated, since the linked list is
				 * created in ascending time order.
				 */
				mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
				mods->sml_op = LDAP_MOD_DELETE;
				mods->sml_flags = SLAP_MOD_INTERNAL;
				mods->sml_type.bv_val = NULL;
				mods->sml_desc = ad_pwdHistory;
				mods->sml_nvalues = NULL;
				mods->sml_values = ch_calloc( sizeof( struct berval ),
											   hsize - pp.pwdInHistory + 2 );
				mods->sml_values[ hsize - pp.pwdInHistory + 1 ].bv_val = NULL;
				mods->sml_values[ hsize - pp.pwdInHistory + 1 ].bv_len = 0;
				for(i=0,p=tl; i < (hsize - pp.pwdInHistory + 1); i++, p=p->next) {
					mods->sml_values[i].bv_val = NULL;
					mods->sml_values[i].bv_len = 0;
					ber_dupbv( &(mods->sml_values[i]), &p->bv );
				}
				mods->sml_next = NULL;
				modtail->sml_next = mods;
				modtail = mods;
			}
			free_pwd_history_list( &tl );

			/*
			 * Now add the existing password into the history list.
			 * This will be executed even if the operation is to delete
			 * the password entirely.
			 *
			 * This isn't in the spec explicitly, but it seems to make
			 * sense that the password history list is the list of all
			 * previous passwords - even if they were deleted. Thus, if
			 * someone tries to add a historical password at some future
			 * point, it will fail.
			 */
			if ((pa = attr_find( e->e_attrs, pp.ad )) != NULL) {
				mods = (Modifications *) ch_malloc( sizeof( Modifications ) );
				mods->sml_op = LDAP_MOD_ADD;
				mods->sml_flags = SLAP_MOD_INTERNAL;
				mods->sml_type.bv_val = NULL;
				mods->sml_desc = ad_pwdHistory;
				mods->sml_nvalues = NULL;
				mods->sml_values = ch_calloc( sizeof( struct berval ), 2 );
				mods->sml_values[ 1 ].bv_val = NULL;
				mods->sml_values[ 1 ].bv_len = 0;
				make_pwd_history_value( timebuf, &mods->sml_values[0], pa );
				mods->sml_next = NULL;
				modtail->sml_next = mods;
				modtail = mods;
			} else {
				Debug( LDAP_DEBUG_TRACE,
				"ppolicy_modify: password attr lookup failed\n", 0, 0, 0 );
			}
		}

		/*
		 * Controversial bit here. If the new password isn't hashed
		 * (ie, is cleartext), we probably should hash it according
		 * to the default hash. The reason for this is that we want
		 * to use the policy if possible, but if we hash the password
		 * before, then we're going to run into trouble when it
		 * comes time to check the password.
		 *
		 * Now, the right thing to do is to use the extended password
		 * modify operation, but not all software can do this,
		 * therefore it makes sense to hash the new password, now
		 * we know it passes the policy requirements.
		 *
		 * Of course, if the password is already hashed, then we
		 * leave it alone.
		 */

		if ((pi->hash_passwords) && (addmod) && !newpw.bv_val && 
			(password_scheme( &(addmod->sml_values[0]), NULL ) != LDAP_SUCCESS)) {
			struct berval hpw, bv;
			
			slap_passwd_hash( &(addmod->sml_values[0]), &hpw, &txt );
			if (hpw.bv_val == NULL) {
					/*
					 * hashing didn't work. Emit an error.
					 */
				rs->sr_err = LDAP_OTHER;
				rs->sr_text = txt;
				goto return_results;
			}
			bv.bv_val = addmod->sml_values[0].bv_val;
			bv.bv_len = addmod->sml_values[0].bv_len;
				/* clear and discard the clear password */
			memset(bv.bv_val, 0, bv.bv_len);
			ber_memfree(bv.bv_val);
			addmod->sml_values[0].bv_val = hpw.bv_val;
			addmod->sml_values[0].bv_len = hpw.bv_len;
		}
	}
	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	be_entry_release_r( op, e );
	return SLAP_CB_CONTINUE;

return_results:
	free_pwd_history_list( &tl );
	op->o_bd->bd_info = (BackendInfo *)on->on_info;
	be_entry_release_r( op, e );
	if ( send_ctrl ) {
		LDAPControl **ctrls = NULL;

		ctrls = ch_calloc( sizeof( LDAPControl *) , 2 );
		ctrls[0] = create_passcontrol( -1, -1, pErr );
		ctrls[1] = NULL;
		rs->sr_ctrls = ctrls;
	}
	send_ldap_result( op, rs );
	return rs->sr_err;
}

static int
ppolicy_parseCtrl(
	Operation *op,
	SlapReply *rs,
	LDAPControl *ctrl )
{
	if ( ctrl->ldctl_value.bv_len ) {
		rs->sr_text = "passwordPolicyRequest control value not empty";
		return LDAP_PROTOCOL_ERROR;
	}
	if ( ctrl->ldctl_iscritical ) {
		rs->sr_text = "passwordPolicyRequest control invalid criticality";
		return LDAP_PROTOCOL_ERROR;
	}
	op->o_ctrlflag[ppolicy_cid] = SLAP_CONTROL_NONCRITICAL;

	return LDAP_SUCCESS;
}

static int
attrPretty(
	Syntax *syntax,
	struct berval *val,
	struct berval *out,
	void *ctx )
{
	AttributeDescription *ad = NULL;
	const char *err;
	int code;

	code = slap_bv2ad( val, &ad, &err );
	if ( !code ) {
		ber_dupbv_x( out, &ad->ad_type->sat_cname, ctx );
	}
	return code;
}

static int
attrNormalize(
	slap_mask_t use,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *out,
	void *ctx )
{
	AttributeDescription *ad = NULL;
	const char *err;
	int code;

	code = slap_bv2ad( val, &ad, &err );
	if ( !code ) {
		ber_str2bv_x( ad->ad_type->sat_oid, 0, 1, out, ctx );
	}
	return code;
}

static int
ppolicy_db_init(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *) be->bd_info;

	/* Has User Schema been initialized yet? */
	if ( !pwd_UsSchema[0].ad[0] ) {
		const char *err;
		int i, code;

		for (i=0; pwd_UsSchema[i].def; i++) {
			code = slap_str2ad( pwd_UsSchema[i].def, pwd_UsSchema[i].ad, &err );
			if ( code ) {
				fprintf( stderr, "User Schema Load failed %d: %s\n", code, err );
				return code;
			}
		}
		{
			Syntax *syn;
			MatchingRule *mr;

			syn = ch_malloc( sizeof( Syntax ));
			*syn = *ad_pwdAttribute->ad_type->sat_syntax;
			syn->ssyn_pretty = attrPretty;
			ad_pwdAttribute->ad_type->sat_syntax = syn;

			mr = ch_malloc( sizeof( MatchingRule ));
			*mr = *ad_pwdAttribute->ad_type->sat_equality;
			mr->smr_normalize = attrNormalize;
			ad_pwdAttribute->ad_type->sat_equality = mr;
		}
	}

	on->on_bi.bi_private = ch_calloc( sizeof(pp_info), 1 );

	if ( dtblsize && !pwcons )
		pwcons = ch_calloc(sizeof(pw_conn), dtblsize );

	return 0;
}

static int
ppolicy_db_open(
    BackendDB *be
)
{
	return overlay_register_control( be, LDAP_CONTROL_PASSWORDPOLICYREQUEST );
}

static int
ppolicy_close(
	BackendDB *be
)
{
	slap_overinst *on = (slap_overinst *) be->bd_info;
	pp_info *pi = on->on_bi.bi_private;
	
	free( pwcons );
	free( pi->def_policy.bv_val );
	free( pi );

	return 0;
}

static int
ppolicy_config(
    BackendDB	*be,
    const char	*fname,
    int		lineno,
    int		argc,
    char	**argv
)
{
	slap_overinst *on = (slap_overinst *) be->bd_info;
	pp_info *pi = on->on_bi.bi_private;
	struct berval dn;
	

	if ( strcasecmp( argv[0], "ppolicy_default" ) == 0 ) {
		if ( argc != 2 ) {
			fprintf( stderr, "%s: line %d: invalid arguments in \"ppolicy_default"
				" <policyDN>\n", fname, lineno );
			return ( 1 );
		}
		ber_str2bv( argv[1], 0, 0, &dn );
		if ( dnNormalize( 0, NULL, NULL, &dn, &pi->def_policy, NULL ) ) {
			fprintf( stderr, "%s: line %d: policyDN is invalid\n",
				fname, lineno );
			return 1;
		}
		return 0;

	} else if ( strcasecmp( argv[0], "ppolicy_use_lockout" ) == 0 ) {
		if ( argc != 1 ) {
			fprintf( stderr, "%s: line %d: ppolicy_use_lockout "
				"takes no arguments\n", fname, lineno );
			return ( 1 );
		}
		pi->use_lockout = 1;
		return 0;
	} else if ( strcasecmp( argv[0], "ppolicy_hash_cleartext" ) == 0 ) {
		if ( argc != 1 ) {
			fprintf( stderr, "%s: line %d: ppolicy_hash_cleartext "
				"takes no arguments\n", fname, lineno );
			return ( 1 );
		}
		pi->hash_passwords = 1;
		return 0;
	}
	return SLAP_CONF_UNKNOWN;
}

static char *extops[] = {
	LDAP_EXOP_MODIFY_PASSWD,
	NULL
};

static slap_overinst ppolicy;

int ppolicy_init()
{
	LDAPAttributeType *at;
	const char *err;
	int i, code;

	for (i=0; pwd_OpSchema[i].def; i++) {
		at = ldap_str2attributetype( pwd_OpSchema[i].def, &code, &err,
			LDAP_SCHEMA_ALLOW_ALL );
		if ( !at ) {
			fprintf( stderr, "AttributeType Load failed %s %s\n",
				ldap_scherr2str(code), err );
			return code;
		}
		code = at_add( at, 0, NULL, &err );
		if ( !code ) {
			slap_str2ad( at->at_names[0], pwd_OpSchema[i].ad, &err );
		}
		ldap_memfree( at );
		if ( code ) {
			fprintf( stderr, "AttributeType Load failed %s %s\n",
				scherr2str(code), err );
			return code;
		}
		/* Allow Manager to set these as needed */
		if ( is_at_no_user_mod( (*pwd_OpSchema[i].ad)->ad_type )) {
			(*pwd_OpSchema[i].ad)->ad_type->sat_flags |=
				SLAP_AT_MANAGEABLE;
		}
	}

	code = register_supported_control( LDAP_CONTROL_PASSWORDPOLICYREQUEST,
		SLAP_CTRL_ADD|SLAP_CTRL_BIND|SLAP_CTRL_MODIFY|SLAP_CTRL_HIDE, extops,
		ppolicy_parseCtrl, &ppolicy_cid );
	if ( code != LDAP_SUCCESS ) {
		fprintf( stderr, "Failed to register control %d\n", code );
		return code;
	}

	ldap_pvt_thread_mutex_init( &chk_syntax_mutex );

	ppolicy.on_bi.bi_type = "ppolicy";
	ppolicy.on_bi.bi_db_init = ppolicy_db_init;
	ppolicy.on_bi.bi_db_open = ppolicy_db_open;
	ppolicy.on_bi.bi_db_config = ppolicy_config;
	ppolicy.on_bi.bi_db_close = ppolicy_close;

	ppolicy.on_bi.bi_op_add = ppolicy_add;
	ppolicy.on_bi.bi_op_bind = ppolicy_bind;
	ppolicy.on_bi.bi_op_compare = ppolicy_restrict;
	ppolicy.on_bi.bi_op_delete = ppolicy_restrict;
	ppolicy.on_bi.bi_op_modify = ppolicy_modify;
	ppolicy.on_bi.bi_op_search = ppolicy_restrict;
	ppolicy.on_bi.bi_connection_destroy = ppolicy_connection_destroy;

	return overlay_register( &ppolicy );
}

#if SLAPD_OVER_PPOLICY == SLAPD_MOD_DYNAMIC
int init_module(int argc, char *argv[]) {
	return ppolicy_init();
}
#endif

#endif	/* defined(SLAPD_OVER_PPOLICY) */
