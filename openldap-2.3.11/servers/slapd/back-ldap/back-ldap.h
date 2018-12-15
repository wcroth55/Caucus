/* back-ldap.h - ldap backend header file */
/* $OpenLDAP: pkg/ldap/servers/slapd/back-ldap/back-ldap.h,v 1.63.2.11 2005/09/28 00:30:32 kurt Exp $ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1999-2005 The OpenLDAP Foundation.
 * Portions Copyright 2000-2003 Pierangelo Masarati.
 * Portions Copyright 1999-2003 Howard Chu.
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
 * This work was initially developed by the Howard Chu for inclusion
 * in OpenLDAP Software and subsequently enhanced by Pierangelo
 * Masarati.
 */

#ifndef SLAPD_LDAP_H
#define SLAPD_LDAP_H

LDAP_BEGIN_DECL

struct ldapconn {
	Connection		*lc_conn;
#define	LDAP_BACK_PCONN		((void *)0x0)
#define	LDAP_BACK_PCONN_TLS	((void *)0x1)
#define	LDAP_BACK_PCONN_ID(c)	((void *)(c) > LDAP_BACK_PCONN_TLS ? (c)->c_connid : -1)
#ifdef HAVE_TLS
#define	LDAP_BACK_PCONN_SET(op)	((op)->o_conn->c_is_tls ? LDAP_BACK_PCONN_TLS : LDAP_BACK_PCONN)
#else /* ! HAVE_TLS */
#define	LDAP_BACK_PCONN_SET(op)	(LDAP_BACK_PCONN)
#endif /* ! HAVE_TLS */

	LDAP			*lc_ld;
	struct berval		lc_cred;
	struct berval 		lc_bound_ndn;
	struct berval		lc_local_ndn;
	unsigned		lc_lcflags;
#define LDAP_BACK_CONN_ISSET(lc,f)	((lc)->lc_lcflags & (f))
#define	LDAP_BACK_CONN_SET(lc,f)	((lc)->lc_lcflags |= (f))
#define	LDAP_BACK_CONN_CLEAR(lc,f)	((lc)->lc_lcflags &= ~(f))
#define	LDAP_BACK_CONN_CPY(lc,f,mlc) \
	do { \
		if ( ((f) & (mlc)->lc_lcflags) == (f) ) { \
			(lc)->lc_lcflags |= (f); \
		} else { \
			(lc)->lc_lcflags &= ~(f); \
		} \
	} while ( 0 )

#define	LDAP_BACK_FCONN_ISBOUND	(0x01)
#define	LDAP_BACK_FCONN_ISANON	(0x02)
#define	LDAP_BACK_FCONN_ISBMASK	(LDAP_BACK_FCONN_ISBOUND|LDAP_BACK_FCONN_ISANON)
#define	LDAP_BACK_FCONN_ISPRIV	(0x04)
#define	LDAP_BACK_FCONN_ISTLS	(0x08)

#define	LDAP_BACK_CONN_ISBOUND(lc)		LDAP_BACK_CONN_ISSET((lc), LDAP_BACK_FCONN_ISBOUND)
#define	LDAP_BACK_CONN_ISBOUND_SET(lc)		LDAP_BACK_CONN_SET((lc), LDAP_BACK_FCONN_ISBOUND)
#define	LDAP_BACK_CONN_ISBOUND_CLEAR(lc)	LDAP_BACK_CONN_CLEAR((lc), LDAP_BACK_FCONN_ISBMASK)
#define	LDAP_BACK_CONN_ISBOUND_CPY(lc, mlc)	LDAP_BACK_CONN_CPY((lc), LDAP_BACK_FCONN_ISBOUND, (mlc))
#define	LDAP_BACK_CONN_ISANON(lc)		LDAP_BACK_CONN_ISSET((lc), LDAP_BACK_FCONN_ISANON)
#define	LDAP_BACK_CONN_ISANON_SET(lc)		LDAP_BACK_CONN_SET((lc), LDAP_BACK_FCONN_ISANON)
#define	LDAP_BACK_CONN_ISANON_CLEAR(lc)		LDAP_BACK_CONN_ISBOUND_CLEAR((lc))
#define	LDAP_BACK_CONN_ISANON_CPY(lc, mlc)	LDAP_BACK_CONN_CPY((lc), LDAP_BACK_FCONN_ISANON, (mlc))
#define	LDAP_BACK_CONN_ISPRIV(lc)		LDAP_BACK_CONN_ISSET((lc), LDAP_BACK_FCONN_ISPRIV)
#define	LDAP_BACK_CONN_ISPRIV_SET(lc)		LDAP_BACK_CONN_SET((lc), LDAP_BACK_FCONN_ISPRIV)
#define	LDAP_BACK_CONN_ISPRIV_CLEAR(lc)		LDAP_BACK_CONN_CLEAR((lc), LDAP_BACK_FCONN_ISPRIV)
#define	LDAP_BACK_CONN_ISPRIV_CPY(lc, mlc)	LDAP_BACK_CONN_CPY((lc), LDAP_BACK_FCONN_ISPRIV, (mlc))
#define	LDAP_BACK_CONN_ISTLS(lc)		LDAP_BACK_CONN_ISSET((lc), LDAP_BACK_FCONN_ISTLS)
#define	LDAP_BACK_CONN_ISTLS_SET(lc)		LDAP_BACK_CONN_SET((lc), LDAP_BACK_FCONN_ISTLS)
#define	LDAP_BACK_CONN_ISTLS_CLEAR(lc)		LDAP_BACK_CONN_CLEAR((lc), LDAP_BACK_FCONN_ISTLS)
#define	LDAP_BACK_CONN_ISTLS_CPY(lc, mlc)	LDAP_BACK_CONN_CPY((lc), LDAP_BACK_FCONN_ISTLS, (mlc))

	unsigned		lc_refcnt;
	unsigned		lc_flags;
};

/*
 * identity assertion modes
 */
enum {
	LDAP_BACK_IDASSERT_LEGACY = 1,
	LDAP_BACK_IDASSERT_NOASSERT,
	LDAP_BACK_IDASSERT_ANONYMOUS,
	LDAP_BACK_IDASSERT_SELF,
	LDAP_BACK_IDASSERT_OTHERDN,
	LDAP_BACK_IDASSERT_OTHERID
};

struct ldapinfo {
	char		*url;
	LDAPURLDesc	*lud;

	slap_bindconf	acl_sb;
#define	acl_authcID	acl_sb.sb_authcId
#define	acl_authcDN	acl_sb.sb_binddn
#define	acl_passwd	acl_sb.sb_cred
#define	acl_authzID	acl_sb.sb_authzId
#define	acl_authmethod	acl_sb.sb_method
#define	acl_sasl_mech	acl_sb.sb_saslmech
#define	acl_sasl_realm	acl_sb.sb_realm
#define	acl_secprops	acl_sb.sb_secprops

	/* ID assert stuff */
	int		idassert_mode;

	slap_bindconf	idassert_sb;
#define	idassert_authcID	idassert_sb.sb_authcId
#define	idassert_authcDN	idassert_sb.sb_binddn
#define	idassert_passwd		idassert_sb.sb_cred
#define	idassert_authzID	idassert_sb.sb_authzId
#define	idassert_authmethod	idassert_sb.sb_method
#define	idassert_sasl_mech	idassert_sb.sb_saslmech
#define	idassert_sasl_realm	idassert_sb.sb_realm
#define	idassert_secprops	idassert_sb.sb_secprops

	unsigned 	idassert_flags;
#define LDAP_BACK_AUTH_NONE		0x00U
#define	LDAP_BACK_AUTH_NATIVE_AUTHZ	0x01U
#define	LDAP_BACK_AUTH_OVERRIDE		0x02U
#define	LDAP_BACK_AUTH_PRESCRIPTIVE	0x04U

	BerVarray	idassert_authz;
	/* end of ID assert stuff */

	int		nretries;
#define LDAP_BACK_RETRY_UNDEFINED	(-2)
#define LDAP_BACK_RETRY_FOREVER		(-1)
#define LDAP_BACK_RETRY_NEVER		(0)
#define LDAP_BACK_RETRY_DEFAULT		(3)

	ldap_pvt_thread_mutex_t		conn_mutex;
	unsigned	flags;
#define LDAP_BACK_F_NONE		0x00U
#define LDAP_BACK_F_SAVECRED		0x01U
#define LDAP_BACK_F_USE_TLS		0x02U
#define LDAP_BACK_F_PROPAGATE_TLS	0x04U
#define LDAP_BACK_F_TLS_CRITICAL	0x08U
#define LDAP_BACK_F_TLS_USE_MASK	(LDAP_BACK_F_USE_TLS|LDAP_BACK_F_TLS_CRITICAL)
#define LDAP_BACK_F_TLS_PROPAGATE_MASK	(LDAP_BACK_F_PROPAGATE_TLS|LDAP_BACK_F_TLS_CRITICAL)
#define LDAP_BACK_F_TLS_MASK		(LDAP_BACK_F_TLS_USE_MASK|LDAP_BACK_F_TLS_PROPAGATE_MASK)
#define LDAP_BACK_F_CHASE_REFERRALS	0x10U
#define LDAP_BACK_F_PROXY_WHOAMI	0x20U

#define	LDAP_BACK_F_SUPPORT_T_F			0x80U
#define	LDAP_BACK_F_SUPPORT_T_F_DISCOVER	0x40U
#define	LDAP_BACK_F_SUPPORT_T_F_MASK		(LDAP_BACK_F_SUPPORT_T_F|LDAP_BACK_F_SUPPORT_T_F_DISCOVER)

#define LDAP_BACK_SAVECRED(li)		( (li)->flags & LDAP_BACK_F_SAVECRED )
#define LDAP_BACK_USE_TLS(li)		( (li)->flags & LDAP_BACK_F_USE_TLS )
#define LDAP_BACK_PROPAGATE_TLS(li)	( (li)->flags & LDAP_BACK_F_PROPAGATE_TLS )
#define LDAP_BACK_TLS_CRITICAL(li)	( (li)->flags & LDAP_BACK_F_TLS_CRITICAL )
#define LDAP_BACK_CHASE_REFERRALS(li)	( (li)->flags & LDAP_BACK_F_CHASE_REFERRALS )

	int		version;

	Avlnode		*conntree;

#if 0
	/* FIXME: automatic rwm instantiation removed */
	int		rwm_started;
#endif
};

typedef enum ldap_back_send_t {
	LDAP_BACK_DONTSEND		= 0x00,
	LDAP_BACK_SENDOK		= 0x01,
	LDAP_BACK_SENDERR		= 0x02,
	LDAP_BACK_SENDRESULT		= (LDAP_BACK_SENDOK|LDAP_BACK_SENDERR)
} ldap_back_send_t;

/* define to use asynchronous StartTLS */
#define SLAP_STARTTLS_ASYNCHRONOUS

/* timeout to use when calling ldap_result() */
#define	LDAP_BACK_RESULT_TIMEOUT	(0)
#define	LDAP_BACK_RESULT_UTIMEOUT	(100000)
#define	LDAP_BACK_TV_SET(tv) \
	do { \
		(tv)->tv_sec = LDAP_BACK_RESULT_TIMEOUT; \
		(tv)->tv_usec = LDAP_BACK_RESULT_UTIMEOUT; \
	} while ( 0 )

LDAP_END_DECL

#include "proto-ldap.h"

#endif /* SLAPD_LDAP_H */
