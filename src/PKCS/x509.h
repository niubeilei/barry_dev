////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __X509__
#define __X509__

#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "CertVerify/crypt_hash.h"
#include "KernelSimu/atomic.h"
#include "PKCS/asn1.h"
#include "aosUtil/Tracer.h"
#include "AppProxy/usbkey.h"

#define DEFAULT_SERIALNO_SIZE	8
#define SERIALNO_BUFSIZE	32
#define MAX_SERIALNO_SIZE	SERIALNO_BUFSIZE  /*256*/



/* The maximum size of a text string (e.g.key owner name) */

#define X509_MAX_TEXTSIZE	64

/* The maximum size of an object attribute.  In theory this can be any size,
   but in practice we limit it to the following maximum to stop people
   creating things like certs containing MPEGs of themselves playing with
   their cat */

#define MIN_ATTRIBUTE_SIZE	12
#define MAX_ATTRIBUTE_SIZE	1024

/* Context-specific tags for certificates */
enum {
	CTAG_CE_VERSION,
	CTAG_CE_ISSUERUNIQUEID,
	CTAG_CE_SUBJECTUNIQUEID,
	CTAG_CE_EXTENSIONS
};

enum {
	CTAG_CL_EXTENSIONS
};


/* Certificate subtypes */
typedef enum {					/* Certificate object types */
	CRYPT_CERTTYPE_NONE,			/* No certificate type */
	CRYPT_CERTTYPE_CERTIFICATE,		/* Certificate */
	CRYPT_CERTTYPE_ATTRIBUTE_CERT,		/* Attribute certificate */
	CRYPT_CERTTYPE_CERTCHAIN,		/* PKCS #7 certificate chain */
	CRYPT_CERTTYPE_CERTREQUEST,		/* PKCS #10 certification request */
	CRYPT_CERTTYPE_REQUEST_CERT,		/* CRMF certification request */
	CRYPT_CERTTYPE_REQUEST_REVOCATION,	/* CRMF revocation request */
	CRYPT_CERTTYPE_CRL,				/* CRL */
	CRYPT_CERTTYPE_CMS_ATTRIBUTES,		/* CMS attributes */
	CRYPT_CERTTYPE_RTCS_REQUEST,		/* RTCS request */
	CRYPT_CERTTYPE_RTCS_RESPONSE,		/* RTCS response */
	CRYPT_CERTTYPE_OCSP_REQUEST,		/* OCSP request */
	CRYPT_CERTTYPE_OCSP_RESPONSE,		/* OCSP response */
	CRYPT_CERTTYPE_PKIUSER,			/* PKI user information */
	/* Alongside the usual types we can also wind up with various
	   certificate-bagging schemes such as cert chains and sequences that
	   can't be exported in this format and therefore aren't visible to the
	   user, but that need to be distinguished internally.  The following
	   types are only visible internally */
	CRYPT_ICERTTYPE_CMS_CERTSET,		/* CMS SET OF Certificate = cert chain */
	CRYPT_ICERTTYPE_SSL_CERTCHAIN,		/* SSL certificate chain = cert chain */
	CRYPT_CERTTYPE_LAST,				/* Last possible cert.type */
	CRYPT_CERTTYPE_LAST_EXTERNAL = CRYPT_CERTTYPE_PKIUSER + 1
} CRYPT_CERTTYPE_TYPE;

/* Algorithms */
typedef enum {
	/* No encryption */
	CRYPT_ALGO_NONE,			/* No encryption */

	/* Conventional encryption */
	CRYPT_ALGO_DES,				/* DES */
	CRYPT_ALGO_3DES,			/* Triple DES */
	CRYPT_ALGO_IDEA,			/* IDEA */
	CRYPT_ALGO_CAST,			/* CAST-128 */
	CRYPT_ALGO_RC2,				/* RC2 */
	CRYPT_ALGO_RC4,				/* RC4 */
	CRYPT_ALGO_RC5,				/* RC5 */
	CRYPT_ALGO_AES,				/* AES */
	CRYPT_ALGO_BLOWFISH,			/* Blowfish */
	CRYPT_ALGO_SKIPJACK,			/* Skipjack */

	/* Public-key encryption */
	CRYPT_ALGO_DH = 100,			/* Diffie-Hellman */
	CRYPT_ALGO_RSA,				/* RSA */
	CRYPT_ALGO_DSA,				/* DSA */
	CRYPT_ALGO_ELGAMAL,			/* ElGamal */
	CRYPT_ALGO_KEA,				/* KEA */

	/* Hash algorithms */
	CRYPT_ALGO_MD2 = 200,			/* MD2 */
	CRYPT_ALGO_MD4,				/* MD4 */
	CRYPT_ALGO_MD5,				/* MD5 */
	CRYPT_ALGO_SHA,				/* SHA/SHA1 */
	CRYPT_ALGO_RIPEMD160,			/* RIPE-MD 160 */
	CRYPT_ALGO_SHA2,			/* SHA2 (SHA-256/384/512)*/

	/* MAC's */
	CRYPT_ALGO_HMAC_MD5 = 300,		/* HMAC-MD5 */
	CRYPT_ALGO_HMAC_SHA,			/* HMAC-SHA */
	CRYPT_ALGO_HMAC_RIPEMD160,		/* HMAC-RIPEMD-160 */

	/* Vendors may want to use their own algorithms that aren't part of the
	   general cryptlib suite.  The following values are for vendor-defined
	   algorithms, and can be used just like the named algorithm types (it's
	   up to the vendor to keep track of what _VENDOR1 actually corresponds
	   to) */
	CRYPT_ALGO_VENDOR1 = 10000,
	CRYPT_ALGO_VENDOR2,
	CRYPT_ALGO_VENDOR3,

	CRYPT_ALGO_LAST,			/* Last possible crypt algo value */

	/* In order that we can scan through a range of algorithms with
	   cryptQueryCapability(), we define the following boundary points for
	   each algorithm class */
	CRYPT_ALGO_FIRST_CONVENTIONAL = CRYPT_ALGO_DES,
	CRYPT_ALGO_LAST_CONVENTIONAL  = CRYPT_ALGO_DH - 1,
	CRYPT_ALGO_FIRST_PKC          = CRYPT_ALGO_DH,
	CRYPT_ALGO_LAST_PKC           = CRYPT_ALGO_MD2 - 1,
	CRYPT_ALGO_FIRST_HASH         = CRYPT_ALGO_MD2,
	CRYPT_ALGO_LAST_HASH          = CRYPT_ALGO_HMAC_MD5 - 1,
	CRYPT_ALGO_FIRST_MAC          = CRYPT_ALGO_HMAC_MD5,
	CRYPT_ALGO_LAST_MAC           = CRYPT_ALGO_HMAC_MD5 + 99	/* End of mac algo.range */
} CRYPT_ALGO_TYPE;

typedef enum {						/* Block cipher modes */
	CRYPT_MODE_NONE,				/* No encryption mode */
	CRYPT_MODE_ECB,					/* ECB */
	CRYPT_MODE_CBC,					/* CBC */
	CRYPT_MODE_CFB,					/* CFB */
	CRYPT_MODE_OFB,					/* OFB */
	CRYPT_MODE_LAST					/* Last possible crypt mode value */
} CRYPT_MODE_TYPE;



typedef enum {
	CRYPT_ATTRIBUTE_NONE,			/* Non-value */

	/*********************/
	/* Object attributes */
	/*********************/
	/* Used internally */
	CRYPT_PROPERTY_FIRST,

	/* Object properties */
	CRYPT_PROPERTY_HIGHSECURITY,		/* Owned+non-forwardcount+locked */
	CRYPT_PROPERTY_OWNER,			/* Object owner */
	CRYPT_PROPERTY_FORWARDCOUNT,		/* No.of times object can be forwarded */
	CRYPT_PROPERTY_LOCKED,			/* Whether properties can be chged/read */
	CRYPT_PROPERTY_USAGECOUNT,		/* Usage count before object expires */
	CRYPT_PROPERTY_NONEXPORTABLE,		/* Whether key is nonexp.from context */

	/* Used internally */
	CRYPT_PROPERTY_LAST, CRYPT_GENERIC_FIRST,

	/* Extended error information */
	CRYPT_ATTRIBUTE_ERRORTYPE,		/* Type of last error */
	CRYPT_ATTRIBUTE_ERRORLOCUS,		/* Locus of last error */
	CRYPT_ATTRIBUTE_INT_ERRORCODE,		/* Low-level software-specific */
	CRYPT_ATTRIBUTE_INT_ERRORMESSAGE, 	/* error code and message */

	/* Generic information */
	CRYPT_ATTRIBUTE_CURRENT_GROUP,		/* Cursor mgt: Group in attribute list */
	CRYPT_ATTRIBUTE_CURRENT,		/* Cursor mgt: Entry in attribute list */
	CRYPT_ATTRIBUTE_CURRENT_INSTANCE,	/* Cursor mgt: Instance in attribute list */
	CRYPT_ATTRIBUTE_BUFFERSIZE,		/* Internal data buffer size */

	/* User internally */
	CRYPT_GENERIC_LAST,

	/****************************/
	/* Configuration attributes */
	/****************************/
	CRYPT_OPTION_FIRST = 100,

	/* cryptlib information (read-only) */
	CRYPT_OPTION_INFO_DESCRIPTION,		/* Text description */
	CRYPT_OPTION_INFO_COPYRIGHT,		/* Copyright notice */
	CRYPT_OPTION_INFO_MAJORVERSION,		/* Major release version */
	CRYPT_OPTION_INFO_MINORVERSION,		/* Minor release version */
	CRYPT_OPTION_INFO_STEPPING,		/* Release stepping */

	/* Encryption options */
	CRYPT_OPTION_ENCR_ALGO,			/* Encryption algorithm */
	CRYPT_OPTION_ENCR_HASH,			/* Hash algorithm */
	CRYPT_OPTION_ENCR_MAC,			/* MAC algorithm */

	/* PKC options */
	CRYPT_OPTION_PKC_ALGO,			/* Public-key encryption algorithm */
	CRYPT_OPTION_PKC_KEYSIZE,		/* Public-key encryption key size */

	/* Signature options */
	CRYPT_OPTION_SIG_ALGO,			/* Signature algorithm */
	CRYPT_OPTION_SIG_KEYSIZE,		/* Signature keysize */

	/* Keying options */
	CRYPT_OPTION_KEYING_ALGO,		/* Key processing algorithm */
	CRYPT_OPTION_KEYING_ITERATIONS,		/* Key processing iterations */

	/* Certificate options */
	CRYPT_OPTION_CERT_SIGNUNRECOGNISEDATTRIBUTES,	/* Whether to sign unrecog.attrs */
	CRYPT_OPTION_CERT_VALIDITY,		/* Certificate validity period */
	CRYPT_OPTION_CERT_UPDATEINTERVAL,	/* CRL update interval */
	CRYPT_OPTION_CERT_COMPLIANCELEVEL,	/* PKIX compliance level for cert chks.*/
	CRYPT_OPTION_CERT_REQUIREPOLICY,	/* Whether explicit policy req'd for certs */

	/* CMS/SMIME options */
	CRYPT_OPTION_CMS_DEFAULTATTRIBUTES,	/* Add default CMS attributes */
	CRYPT_OPTION_SMIME_DEFAULTATTRIBUTES = CRYPT_OPTION_CMS_DEFAULTATTRIBUTES,

	/* LDAP keyset options */
	CRYPT_OPTION_KEYS_LDAP_OBJECTCLASS,	/* Object class */
	CRYPT_OPTION_KEYS_LDAP_OBJECTTYPE,	/* Object type to fetch */
	CRYPT_OPTION_KEYS_LDAP_FILTER,		/* Query filter */
	CRYPT_OPTION_KEYS_LDAP_CACERTNAME,	/* CA certificate attribute name */
	CRYPT_OPTION_KEYS_LDAP_CERTNAME,	/* Certificate attribute name */
	CRYPT_OPTION_KEYS_LDAP_CRLNAME,		/* CRL attribute name */
	CRYPT_OPTION_KEYS_LDAP_EMAILNAME,	/* Email attribute name */

	/* Crypto device options */
	CRYPT_OPTION_DEVICE_PKCS11_DVR01,	/* Name of first PKCS #11 driver */
	CRYPT_OPTION_DEVICE_PKCS11_DVR02,	/* Name of second PKCS #11 driver */
	CRYPT_OPTION_DEVICE_PKCS11_DVR03,	/* Name of third PKCS #11 driver */
	CRYPT_OPTION_DEVICE_PKCS11_DVR04,	/* Name of fourth PKCS #11 driver */
	CRYPT_OPTION_DEVICE_PKCS11_DVR05,	/* Name of fifth PKCS #11 driver */
	CRYPT_OPTION_DEVICE_PKCS11_HARDWAREONLY,/* Use only hardware mechanisms */

	/* Network access options */
	CRYPT_OPTION_NET_SOCKS_SERVER,		/* Socks server name */
	CRYPT_OPTION_NET_SOCKS_USERNAME,	/* Socks user name */
	CRYPT_OPTION_NET_HTTP_PROXY,		/* Web proxy server */
	CRYPT_OPTION_NET_CONNECTTIMEOUT,	/* Timeout for network connection setup */
	CRYPT_OPTION_NET_READTIMEOUT,		/* Timeout for network reads */
	CRYPT_OPTION_NET_WRITETIMEOUT,		/* Timeout for network writes */

	/* Miscellaneous options */
	CRYPT_OPTION_MISC_ASYNCINIT,		/* Whether to init cryptlib async'ly */
	CRYPT_OPTION_MISC_SIDECHANNELPROTECTION,/* Protect against side-channel attacks */

	/* cryptlib state information */
	CRYPT_OPTION_CONFIGCHANGED,		/* Whether in-mem.opts match on-disk ones */
	CRYPT_OPTION_SELFTESTOK,		/* Whether self-test was completed and OK */

	/* Used internally */
	CRYPT_OPTION_LAST,

	/**********************/
	/* Context attributes */
	/**********************/
	CRYPT_CTXINFO_FIRST = 1000,

	/* Algorithm and mode information */
	CRYPT_CTXINFO_ALGO,			/* Algorithm */
	CRYPT_CTXINFO_MODE,			/* Mode */
	CRYPT_CTXINFO_NAME_ALGO,		/* Algorithm name */
	CRYPT_CTXINFO_NAME_MODE,		/* Mode name */
	CRYPT_CTXINFO_KEYSIZE,			/* Key size in bytes */
	CRYPT_CTXINFO_BLOCKSIZE,		/* Block size */
	CRYPT_CTXINFO_IVSIZE,			/* IV size */
	CRYPT_CTXINFO_KEYING_ALGO,		/* Key processing algorithm */
	CRYPT_CTXINFO_KEYING_ITERATIONS,	/* Key processing iterations */
	CRYPT_CTXINFO_KEYING_SALT,		/* Key processing salt */
	CRYPT_CTXINFO_KEYING_VALUE,		/* Value used to derive key */

	/* State information */
	CRYPT_CTXINFO_KEY,			/* Key */
	CRYPT_CTXINFO_KEY_COMPONENTS,		/* Public-key components */
	CRYPT_CTXINFO_IV,			/* IV */
	CRYPT_CTXINFO_HASHVALUE,		/* Hash value */

	/* Misc.information */
	CRYPT_CTXINFO_LABEL,			/* Label for private/secret key */

	/* Used internally */
	CRYPT_CTXINFO_LAST,

	/**************************/
	/* Certificate attributes */
	/**************************/
	CRYPT_CERTINFO_FIRST = 2000,

	/* Because there are so many cert attributes, we break them down into
	   blocks to minimise the number of values that change if a new one is
	   added halfway through */

	/* Pseudo-information on a cert object or meta-information which is used
	   to control the way that a cert object is processed */
	CRYPT_CERTINFO_SELFSIGNED,		/* Cert is self-signed */
	CRYPT_CERTINFO_IMMUTABLE,		/* Cert is signed and immutable */
	CRYPT_CERTINFO_XYZZY,			/* Cert is a magic just-works cert */
	CRYPT_CERTINFO_CERTTYPE,		/* Certificate object type */
	CRYPT_CERTINFO_FINGERPRINT,		/* Certificate fingerprints */
	CRYPT_CERTINFO_FINGERPRINT_MD5 = CRYPT_CERTINFO_FINGERPRINT,
	CRYPT_CERTINFO_FINGERPRINT_SHA,
	CRYPT_CERTINFO_CURRENT_CERTIFICATE,	/* Cursor mgt: Rel.pos in chain/CRL/OCSP */
#if 1	/* To be removed in cryptlib 3.2 */
	CRYPT_CERTINFO_CURRENT_EXTENSION,	/* Cursor mgt: Rel.pos.or abs.extension */
	CRYPT_CERTINFO_CURRENT_FIELD,		/* Cursor mgt: Rel.pos.or abs.field in ext */
	CRYPT_CERTINFO_CURRENT_COMPONENT,	/* Cursor mgt: Rel.pos in multival.field */
#endif /* 1 */
	CRYPT_CERTINFO_TRUSTED_USAGE,		/* Usage that cert is trusted for */
	CRYPT_CERTINFO_TRUSTED_IMPLICIT,	/* Whether cert is implicitly trusted */
	CRYPT_CERTINFO_SIGNATURELEVEL,		/* Amount of detail to include in sigs.*/

	/* General certificate object information */
	CRYPT_CERTINFO_VERSION,			/* Cert.format version */
	CRYPT_CERTINFO_SERIALNUMBER,		/* Serial number */
	CRYPT_CERTINFO_SUBJECTPUBLICKEYINFO,	/* Public key */
	CRYPT_CERTINFO_CERTIFICATE,		/* User certificate */
	CRYPT_CERTINFO_USERCERTIFICATE = CRYPT_CERTINFO_CERTIFICATE,
	CRYPT_CERTINFO_CACERTIFICATE,		/* CA certificate */
	CRYPT_CERTINFO_ISSUERNAME,		/* Issuer DN */
	CRYPT_CERTINFO_VALIDFROM,		/* Cert valid-from time */
	CRYPT_CERTINFO_VALIDTO,			/* Cert valid-to time */
	CRYPT_CERTINFO_SUBJECTNAME,		/* Subject DN */
	CRYPT_CERTINFO_ISSUERUNIQUEID,		/* Issuer unique ID */
	CRYPT_CERTINFO_SUBJECTUNIQUEID,		/* Subject unique ID */
	CRYPT_CERTINFO_CERTREQUEST,		/* Cert.request (DN + public key) */
	CRYPT_CERTINFO_THISUPDATE,		/* CRL/OCSP current-update time */
	CRYPT_CERTINFO_NEXTUPDATE,		/* CRL/OCSP next-update time */
	CRYPT_CERTINFO_REVOCATIONDATE,		/* CRL/OCSP cert-revocation time */
	CRYPT_CERTINFO_REVOCATIONSTATUS,	/* OCSP revocation status */
	CRYPT_CERTINFO_CERTSTATUS,		/* RTCS certificate status */
	CRYPT_CERTINFO_DN,			/* Currently selected DN in string form */
	CRYPT_CERTINFO_PKIUSER_ID,		/* PKI user ID */
	CRYPT_CERTINFO_PKIUSER_ISSUEPASSWORD,	/* PKI user issue password */
	CRYPT_CERTINFO_PKIUSER_REVPASSWORD,	/* PKI user revocation password */

	/* X.520 Distinguished Name components.  This is a composite field, the
	   DN to be manipulated is selected through the addition of a
	   pseudocomponent, and then one of the following is used to access the
	   DN components directly */
	CRYPT_CERTINFO_COUNTRYNAME = CRYPT_CERTINFO_FIRST + 100,	/* countryName */
	CRYPT_CERTINFO_STATEORPROVINCENAME,		/* stateOrProvinceName */
	CRYPT_CERTINFO_LOCALITYNAME,			/* localityName */
	CRYPT_CERTINFO_ORGANIZATIONNAME,		/* organizationName */
	CRYPT_CERTINFO_ORGANISATIONNAME = CRYPT_CERTINFO_ORGANIZATIONNAME,
	CRYPT_CERTINFO_ORGANIZATIONALUNITNAME,		/* organizationalUnitName */
	CRYPT_CERTINFO_ORGANISATIONALUNITNAME = CRYPT_CERTINFO_ORGANIZATIONALUNITNAME,
	CRYPT_CERTINFO_COMMONNAME,			/* commonName */
	CRYPT_CERTINFO_EMAILADDR,			/* XXX: add by support email of DN */

	/* X.509 General Name components.  These are handled in the same way as
	   the DN composite field, with the current GeneralName being selected by
	   a pseudo-component after which the individual components can be
	   modified through one of the following */
	CRYPT_CERTINFO_OTHERNAME_TYPEID,		/* otherName.typeID */
	CRYPT_CERTINFO_OTHERNAME_VALUE,			/* otherName.value */
	CRYPT_CERTINFO_RFC822NAME,			/* rfc822Name */
	CRYPT_CERTINFO_EMAIL = CRYPT_CERTINFO_RFC822NAME,
	CRYPT_CERTINFO_DNSNAME,				/* dNSName */
	CRYPT_CERTINFO_DIRECTORYNAME,			/* directoryName */
	CRYPT_CERTINFO_EDIPARTYNAME_NAMEASSIGNER,	/* ediPartyName.nameAssigner */
	CRYPT_CERTINFO_EDIPARTYNAME_PARTYNAME,		/* ediPartyName.partyName */
	CRYPT_CERTINFO_UNIFORMRESOURCEIDENTIFIER,	/* uniformResourceIdentifier */
	CRYPT_CERTINFO_IPADDRESS,			/* iPAddress */
	CRYPT_CERTINFO_REGISTEREDID,			/* registeredID */

	/* X.509 certificate extensions.  Although it would be nicer to use names
	   that match the extensions more closely (e.g.
	   CRYPT_CERTINFO_BASICCONSTRAINTS_PATHLENCONSTRAINT), these exceed the
	   32-character ANSI minimum length for unique names, and get really
	   hairy once you get into the weird policy constraints extensions whose
	   names wrap around the screen about three times.

	   The following values are defined in OID order, this isn't absolutely
	   necessary but saves an extra layer of processing when encoding them */

	/* 1 2 840 113549 1 9 7 challengePassword.  This is here even though it's
	   a CMS attribute because SCEP stuffs it into PKCS #10 requests */
	CRYPT_CERTINFO_CHALLENGEPASSWORD = CRYPT_CERTINFO_FIRST + 200,

	/* 1 3 6 1 4 1 3029 3 1 4 cRLExtReason */
	CRYPT_CERTINFO_CRLEXTREASON,

	/* 1 3 6 1 4 1 3029 3 1 5 keyFeatures */
	CRYPT_CERTINFO_KEYFEATURES,

	/* 1 3 6 1 5 5 7 1 1 authorityInfoAccess */
	CRYPT_CERTINFO_AUTHORITYINFOACCESS,
	CRYPT_CERTINFO_AUTHORITYINFO_RTCS,		/* accessDescription.accessLocation */
	CRYPT_CERTINFO_AUTHORITYINFO_OCSP,		/* accessDescription.accessLocation */
	CRYPT_CERTINFO_AUTHORITYINFO_CAISSUERS,		/* accessDescription.accessLocation */
	CRYPT_CERTINFO_AUTHORITYINFO_CERTSTORE,		/* accessDescription.accessLocation */
	CRYPT_CERTINFO_AUTHORITYINFO_CRLS,		/* accessDescription.accessLocation */

	/* 1 3 6 1 5 5 7 1 2 biometricInfo */
	CRYPT_CERTINFO_BIOMETRICINFO,
	CRYPT_CERTINFO_BIOMETRICINFO_TYPE,		/* biometricData.typeOfData */
	CRYPT_CERTINFO_BIOMETRICINFO_HASHALGO,		/* biometricData.hashAlgorithm */
	CRYPT_CERTINFO_BIOMETRICINFO_HASH,		/* biometricData.dataHash */
	CRYPT_CERTINFO_BIOMETRICINFO_URL,		/* biometricData.sourceDataUri */

	/* 1 3 6 1 5 5 7 1 3 qcStatements */
	CRYPT_CERTINFO_QCSTATEMENT,
	CRYPT_CERTINFO_QCSTATEMENT_SEMANTICS,
					/* qcStatement.statementInfo.semanticsIdentifier */
	CRYPT_CERTINFO_QCSTATEMENT_REGISTRATIONAUTHORITY,
					/* qcStatement.statementInfo.nameRegistrationAuthorities */

	/* 1 3 6 1 5 5 7 48 1 2 ocspNonce */
	CRYPT_CERTINFO_OCSP_NONCE,			/* nonce */

	/* 1 3 6 1 5 5 7 48 1 4 ocspAcceptableResponses */
	CRYPT_CERTINFO_OCSP_RESPONSE,
	CRYPT_CERTINFO_OCSP_RESPONSE_OCSP,		/* OCSP standard response */

	/* 1 3 6 1 5 5 7 48 1 5 ocspNoCheck */
	CRYPT_CERTINFO_OCSP_NOCHECK,

	/* 1 3 6 1 5 5 7 48 1 6 ocspArchiveCutoff */
	CRYPT_CERTINFO_OCSP_ARCHIVECUTOFF,

	/* 1 3 6 1 5 5 7 48 1 11 subjectInfoAccess */
	CRYPT_CERTINFO_SUBJECTINFOACCESS,
	CRYPT_CERTINFO_SUBJECTINFO_CAREPOSITORY,	/* accessDescription.accessLocation */
	CRYPT_CERTINFO_SUBJECTINFO_TIMESTAMPING,	/* accessDescription.accessLocation */

	/* 1 3 36 8 3 1 siggDateOfCertGen */
	CRYPT_CERTINFO_SIGG_DATEOFCERTGEN,

	/* 1 3 36 8 3 2 siggProcuration */
	CRYPT_CERTINFO_SIGG_PROCURATION,
	CRYPT_CERTINFO_SIGG_PROCURE_COUNTRY,		/* country */
	CRYPT_CERTINFO_SIGG_PROCURE_TYPEOFSUBSTITUTION,	/* typeOfSubstitution */
	CRYPT_CERTINFO_SIGG_PROCURE_SIGNINGFOR,		/* signingFor.thirdPerson */

	/* 1 3 36 8 3 4 siggMonetaryLimit */
	CRYPT_CERTINFO_SIGG_MONETARYLIMIT,
	CRYPT_CERTINFO_SIGG_MONETARY_CURRENCY,		/* currency */
	CRYPT_CERTINFO_SIGG_MONETARY_AMOUNT,		/* amount */
	CRYPT_CERTINFO_SIGG_MONETARY_EXPONENT,		/* exponent */

	/* 1 3 36 8 3 8 siggRestriction */
	CRYPT_CERTINFO_SIGG_RESTRICTION,

	/* 1 3 101 1 4 1 strongExtranet */
	CRYPT_CERTINFO_STRONGEXTRANET,
	CRYPT_CERTINFO_STRONGEXTRANET_ZONE,		/* sxNetIDList.sxNetID.zone */
	CRYPT_CERTINFO_STRONGEXTRANET_ID,		/* sxNetIDList.sxNetID.id */

	/* 2 5 29 9 subjectDirectoryAttributes */
	CRYPT_CERTINFO_SUBJECTDIRECTORYATTRIBUTES,
	CRYPT_CERTINFO_SUBJECTDIR_TYPE,			/* attribute.type */
	CRYPT_CERTINFO_SUBJECTDIR_VALUES,		/* attribute.values */

	/* 2 5 29 14 subjectKeyIdentifier */
	CRYPT_CERTINFO_SUBJECTKEYIDENTIFIER,

	/* 2 5 29 15 keyUsage */
	CRYPT_CERTINFO_KEYUSAGE,

	/* 2 5 29 16 privateKeyUsagePeriod */
	CRYPT_CERTINFO_PRIVATEKEYUSAGEPERIOD,
	CRYPT_CERTINFO_PRIVATEKEY_NOTBEFORE,		/* notBefore */
	CRYPT_CERTINFO_PRIVATEKEY_NOTAFTER,		/* notAfter */

	/* 2 5 29 17 subjectAltName */
	CRYPT_CERTINFO_SUBJECTALTNAME,

	/* 2 5 29 18 issuerAltName */
	CRYPT_CERTINFO_ISSUERALTNAME,

	/* 2 5 29 19 basicConstraints */
	CRYPT_CERTINFO_BASICCONSTRAINTS,
	CRYPT_CERTINFO_CA,				/* cA */
		CRYPT_CERTINFO_AUTHORITY = CRYPT_CERTINFO_CA,
	CRYPT_CERTINFO_PATHLENCONSTRAINT,		/* pathLenConstraint */

	/* 2 5 29 20 cRLNumber */
	CRYPT_CERTINFO_CRLNUMBER,

	/* 2 5 29 21 cRLReason */
	CRYPT_CERTINFO_CRLREASON,

	/* 2 5 29 23 holdInstructionCode */
	CRYPT_CERTINFO_HOLDINSTRUCTIONCODE,

	/* 2 5 29 24 invalidityDate */
	CRYPT_CERTINFO_INVALIDITYDATE,

	/* 2 5 29 27 deltaCRLIndicator */
	CRYPT_CERTINFO_DELTACRLINDICATOR,

	/* 2 5 29 28 issuingDistributionPoint */
	CRYPT_CERTINFO_ISSUINGDISTRIBUTIONPOINT,
	CRYPT_CERTINFO_ISSUINGDIST_FULLNAME,		/* distributionPointName.fullName */
	CRYPT_CERTINFO_ISSUINGDIST_USERCERTSONLY,	/* onlyContainsUserCerts */
	CRYPT_CERTINFO_ISSUINGDIST_CACERTSONLY,		/* onlyContainsCACerts */
	CRYPT_CERTINFO_ISSUINGDIST_SOMEREASONSONLY,	/* onlySomeReasons */
	CRYPT_CERTINFO_ISSUINGDIST_INDIRECTCRL,		/* indirectCRL */

	/* 2 5 29 29 certificateIssuer */
	CRYPT_CERTINFO_CERTIFICATEISSUER,

	/* 2 5 29 30 nameConstraints */
	CRYPT_CERTINFO_NAMECONSTRAINTS,
	CRYPT_CERTINFO_PERMITTEDSUBTREES,		/* permittedSubtrees */
	CRYPT_CERTINFO_EXCLUDEDSUBTREES,		/* excludedSubtrees */

	/* 2 5 29 31 cRLDistributionPoint */
	CRYPT_CERTINFO_CRLDISTRIBUTIONPOINT,
	CRYPT_CERTINFO_CRLDIST_FULLNAME,		/* distributionPointName.fullName */
	CRYPT_CERTINFO_CRLDIST_REASONS,			/* reasons */
	CRYPT_CERTINFO_CRLDIST_CRLISSUER,		/* cRLIssuer */

	/* 2 5 29 32 certificatePolicies */
	CRYPT_CERTINFO_CERTIFICATEPOLICIES,
	CRYPT_CERTINFO_CERTPOLICYID,			/* policyInformation.policyIdentifier */
	CRYPT_CERTINFO_CERTPOLICY_CPSURI,
		/* policyInformation.policyQualifiers.qualifier.cPSuri */
	CRYPT_CERTINFO_CERTPOLICY_ORGANIZATION,
		/* policyInformation.policyQualifiers.qualifier.userNotice.noticeRef.organization */
	CRYPT_CERTINFO_CERTPOLICY_NOTICENUMBERS,
		/* policyInformation.policyQualifiers.qualifier.userNotice.noticeRef.noticeNumbers */
	CRYPT_CERTINFO_CERTPOLICY_EXPLICITTEXT,
		/* policyInformation.policyQualifiers.qualifier.userNotice.explicitText */

	/* 2 5 29 33 policyMappings */
	CRYPT_CERTINFO_POLICYMAPPINGS,
	CRYPT_CERTINFO_ISSUERDOMAINPOLICY,	/* policyMappings.issuerDomainPolicy */
	CRYPT_CERTINFO_SUBJECTDOMAINPOLICY,	/* policyMappings.subjectDomainPolicy */

	/* 2 5 29 35 authorityKeyIdentifier */
	CRYPT_CERTINFO_AUTHORITYKEYIDENTIFIER,
	CRYPT_CERTINFO_AUTHORITY_KEYIDENTIFIER,	/* keyIdentifier */
	CRYPT_CERTINFO_AUTHORITY_CERTISSUER,	/* authorityCertIssuer */
	CRYPT_CERTINFO_AUTHORITY_CERTSERIALNUMBER,	/* authorityCertSerialNumber */

	/* 2 5 29 36 policyConstraints */
	CRYPT_CERTINFO_POLICYCONSTRAINTS,
	CRYPT_CERTINFO_REQUIREEXPLICITPOLICY,	/* policyConstraints.requireExplicitPolicy */
	CRYPT_CERTINFO_INHIBITPOLICYMAPPING,	/* policyConstraints.inhibitPolicyMapping */

	/* 2 5 29 37 extKeyUsage */
	CRYPT_CERTINFO_EXTKEYUSAGE,
	CRYPT_CERTINFO_EXTKEY_MS_INDIVIDUALCODESIGNING,	/* individualCodeSigning */
	CRYPT_CERTINFO_EXTKEY_MS_COMMERCIALCODESIGNING,	/* commercialCodeSigning */
	CRYPT_CERTINFO_EXTKEY_MS_CERTTRUSTLISTSIGNING,	/* certTrustListSigning */
	CRYPT_CERTINFO_EXTKEY_MS_TIMESTAMPSIGNING,	/* timeStampSigning */
	CRYPT_CERTINFO_EXTKEY_MS_SERVERGATEDCRYPTO,	/* serverGatedCrypto */
	CRYPT_CERTINFO_EXTKEY_MS_ENCRYPTEDFILESYSTEM,	/* encrypedFileSystem */
	CRYPT_CERTINFO_EXTKEY_SERVERAUTH,		/* serverAuth */
	CRYPT_CERTINFO_EXTKEY_CLIENTAUTH,		/* clientAuth */
	CRYPT_CERTINFO_EXTKEY_CODESIGNING,		/* codeSigning */
	CRYPT_CERTINFO_EXTKEY_EMAILPROTECTION,		/* emailProtection */
	CRYPT_CERTINFO_EXTKEY_IPSECENDSYSTEM,		/* ipsecEndSystem */
	CRYPT_CERTINFO_EXTKEY_IPSECTUNNEL,		/* ipsecTunnel */
	CRYPT_CERTINFO_EXTKEY_IPSECUSER,		/* ipsecUser */
	CRYPT_CERTINFO_EXTKEY_TIMESTAMPING,		/* timeStamping */
	CRYPT_CERTINFO_EXTKEY_OCSPSIGNING,		/* ocspSigning */
	CRYPT_CERTINFO_EXTKEY_DIRECTORYSERVICE,		/* directoryService */
	CRYPT_CERTINFO_EXTKEY_ANYKEYUSAGE,		/* anyExtendedKeyUsage */
	CRYPT_CERTINFO_EXTKEY_NS_SERVERGATEDCRYPTO,	/* serverGatedCrypto */
	CRYPT_CERTINFO_EXTKEY_VS_SERVERGATEDCRYPTO_CA,	/* serverGatedCrypto CA */

	/* 2 5 29 46 freshestCRL */
	CRYPT_CERTINFO_FRESHESTCRL,
	CRYPT_CERTINFO_FRESHESTCRL_FULLNAME,		/* distributionPointName.fullName */
	CRYPT_CERTINFO_FRESHESTCRL_REASONS,		/* reasons */
	CRYPT_CERTINFO_FRESHESTCRL_CRLISSUER,		/* cRLIssuer */

	/* 2 5 29 54 inhibitAnyPolicy */
	CRYPT_CERTINFO_INHIBITANYPOLICY,

	/* 2 16 840 1 113730 1 x Netscape extensions */
	CRYPT_CERTINFO_NS_CERTTYPE,			/* netscape-cert-type */
	CRYPT_CERTINFO_NS_BASEURL,			/* netscape-base-url */
	CRYPT_CERTINFO_NS_REVOCATIONURL,		/* netscape-revocation-url */
	CRYPT_CERTINFO_NS_CAREVOCATIONURL,		/* netscape-ca-revocation-url */
	CRYPT_CERTINFO_NS_CERTRENEWALURL,		/* netscape-cert-renewal-url */
	CRYPT_CERTINFO_NS_CAPOLICYURL,			/* netscape-ca-policy-url */
	CRYPT_CERTINFO_NS_SSLSERVERNAME,		/* netscape-ssl-server-name */
	CRYPT_CERTINFO_NS_COMMENT,			/* netscape-comment */

	/* 2 23 42 7 0 SET hashedRootKey */
	CRYPT_CERTINFO_SET_HASHEDROOTKEY,
	CRYPT_CERTINFO_SET_ROOTKEYTHUMBPRINT,		/* rootKeyThumbPrint */

	/* 2 23 42 7 1 SET certificateType */
	CRYPT_CERTINFO_SET_CERTIFICATETYPE,

	/* 2 23 42 7 2 SET merchantData */
	CRYPT_CERTINFO_SET_MERCHANTDATA,
	CRYPT_CERTINFO_SET_MERID,			/* merID */
	CRYPT_CERTINFO_SET_MERACQUIRERBIN,		/* merAcquirerBIN */
	CRYPT_CERTINFO_SET_MERCHANTLANGUAGE,		/* merNames.language */
	CRYPT_CERTINFO_SET_MERCHANTNAME,		/* merNames.name */
	CRYPT_CERTINFO_SET_MERCHANTCITY,		/* merNames.city */
	CRYPT_CERTINFO_SET_MERCHANTSTATEPROVINCE,	/* merNames.stateProvince */
	CRYPT_CERTINFO_SET_MERCHANTPOSTALCODE,		/* merNames.postalCode */
	CRYPT_CERTINFO_SET_MERCHANTCOUNTRYNAME,		/* merNames.countryName */
	CRYPT_CERTINFO_SET_MERCOUNTRY,			/* merCountry */
	CRYPT_CERTINFO_SET_MERAUTHFLAG,			/* merAuthFlag */

	/* 2 23 42 7 3 SET certCardRequired */
	CRYPT_CERTINFO_SET_CERTCARDREQUIRED,

	/* 2 23 42 7 4 SET tunneling */
	CRYPT_CERTINFO_SET_TUNNELING,
	CRYPT_CERTINFO_SET_TUNNELLING = CRYPT_CERTINFO_SET_TUNNELING,
	CRYPT_CERTINFO_SET_TUNNELINGFLAG,		/* tunneling */
	CRYPT_CERTINFO_SET_TUNNELLINGFLAG = CRYPT_CERTINFO_SET_TUNNELINGFLAG,
	CRYPT_CERTINFO_SET_TUNNELINGALGID,		/* tunnelingAlgID */
	CRYPT_CERTINFO_SET_TUNNELLINGALGID = CRYPT_CERTINFO_SET_TUNNELINGALGID,

	/* S/MIME attributes */

	/* 1 2 840 113549 1 9 3 contentType */
	CRYPT_CERTINFO_CMS_CONTENTTYPE = CRYPT_CERTINFO_FIRST + 500,

	/* 1 2 840 113549 1 9 4 messageDigest */
	CRYPT_CERTINFO_CMS_MESSAGEDIGEST,

	/* 1 2 840 113549 1 9 5 signingTime */
	CRYPT_CERTINFO_CMS_SIGNINGTIME,

	/* 1 2 840 113549 1 9 6 counterSignature */
	CRYPT_CERTINFO_CMS_COUNTERSIGNATURE,	/* counterSignature */

	/* 1 2 840 113549 1 9 13 signingDescription */
	CRYPT_CERTINFO_CMS_SIGNINGDESCRIPTION,

	/* 1 2 840 113549 1 9 15 sMIMECapabilities */
	CRYPT_CERTINFO_CMS_SMIMECAPABILITIES,
	CRYPT_CERTINFO_CMS_SMIMECAP_3DES,		/* 3DES encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_AES,		/* AES encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_CAST128,		/* CAST-128 encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_IDEA,		/* IDEA encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_RC2,		/* RC2 encryption (w.128 key) */
	CRYPT_CERTINFO_CMS_SMIMECAP_RC5,		/* RC5 encryption (w.128 key) */
	CRYPT_CERTINFO_CMS_SMIMECAP_SKIPJACK,		/* Skipjack encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_DES,		/* DES encryption */
	CRYPT_CERTINFO_CMS_SMIMECAP_PREFERSIGNEDDATA,	/* preferSignedData */
	CRYPT_CERTINFO_CMS_SMIMECAP_CANNOTDECRYPTANY,	/* canNotDecryptAny */

	/* 1 2 840 113549 1 9 16 2 1 receiptRequest */
	CRYPT_CERTINFO_CMS_RECEIPTREQUEST,
	CRYPT_CERTINFO_CMS_RECEIPT_CONTENTIDENTIFIER,	/* contentIdentifier */
	CRYPT_CERTINFO_CMS_RECEIPT_FROM,		/* receiptsFrom */
	CRYPT_CERTINFO_CMS_RECEIPT_TO,			/* receiptsTo */

	/* 1 2 840 113549 1 9 16 2 2 essSecurityLabel */
	CRYPT_CERTINFO_CMS_SECURITYLABEL,
	CRYPT_CERTINFO_CMS_SECLABEL_CLASSIFICATION, 	/* securityClassification */
	CRYPT_CERTINFO_CMS_SECLABEL_POLICY,		/* securityPolicyIdentifier */
	CRYPT_CERTINFO_CMS_SECLABEL_PRIVACYMARK,	/* privacyMark */
	CRYPT_CERTINFO_CMS_SECLABEL_CATTYPE,		/* securityCategories.securityCategory.type */
	CRYPT_CERTINFO_CMS_SECLABEL_CATVALUE,		/* securityCategories.securityCategory.value */

	/* 1 2 840 113549 1 9 16 2 3 mlExpansionHistory */
	CRYPT_CERTINFO_CMS_MLEXPANSIONHISTORY,
	CRYPT_CERTINFO_CMS_MLEXP_ENTITYIDENTIFIER, 	/* mlData.mailListIdentifier.issuerAndSerialNumber */
	CRYPT_CERTINFO_CMS_MLEXP_TIME,			/* mlData.expansionTime */
	CRYPT_CERTINFO_CMS_MLEXP_NONE,			/* mlData.mlReceiptPolicy.none */
	CRYPT_CERTINFO_CMS_MLEXP_INSTEADOF,		/* mlData.mlReceiptPolicy.insteadOf.generalNames.generalName */
	CRYPT_CERTINFO_CMS_MLEXP_INADDITIONTO,		/* mlData.mlReceiptPolicy.inAdditionTo.generalNames.generalName */

	/* 1 2 840 113549 1 9 16 2 4 contentHints */
	CRYPT_CERTINFO_CMS_CONTENTHINTS,
	CRYPT_CERTINFO_CMS_CONTENTHINT_DESCRIPTION,	/* contentDescription */
	CRYPT_CERTINFO_CMS_CONTENTHINT_TYPE,		/* contentType */

	/* 1 2 840 113549 1 9 16 2 9 equivalentLabels */
	CRYPT_CERTINFO_CMS_EQUIVALENTLABEL,
	CRYPT_CERTINFO_CMS_EQVLABEL_POLICY,		/* securityPolicyIdentifier */
	CRYPT_CERTINFO_CMS_EQVLABEL_CLASSIFICATION, 	/* securityClassification */
	CRYPT_CERTINFO_CMS_EQVLABEL_PRIVACYMARK,	/* privacyMark */
	CRYPT_CERTINFO_CMS_EQVLABEL_CATTYPE,		/* securityCategories.securityCategory.type */
	CRYPT_CERTINFO_CMS_EQVLABEL_CATVALUE,		/* securityCategories.securityCategory.value */

	/* 1 2 840 113549 1 9 16 2 12 signingCertificate */
	CRYPT_CERTINFO_CMS_SIGNINGCERTIFICATE,
	CRYPT_CERTINFO_CMS_SIGNINGCERT_ESSCERTID, 	/* certs.essCertID */
	CRYPT_CERTINFO_CMS_SIGNINGCERT_POLICIES,	/* policies.policyInformation.policyIdentifier */

	/* 1 2 840 113549 1 9 16 2 15 signaturePolicyID */
	CRYPT_CERTINFO_CMS_SIGNATUREPOLICYID,
	CRYPT_CERTINFO_CMS_SIGPOLICYID,			/* sigPolicyID */
	CRYPT_CERTINFO_CMS_SIGPOLICYHASH,		/* sigPolicyHash */
	CRYPT_CERTINFO_CMS_SIGPOLICY_CPSURI,		/* sigPolicyQualifiers.sigPolicyQualifier.cPSuri */
	CRYPT_CERTINFO_CMS_SIGPOLICY_ORGANIZATION,
		/* sigPolicyQualifiers.sigPolicyQualifier.userNotice.noticeRef.organization */
	CRYPT_CERTINFO_CMS_SIGPOLICY_NOTICENUMBERS,
		/* sigPolicyQualifiers.sigPolicyQualifier.userNotice.noticeRef.noticeNumbers */
	CRYPT_CERTINFO_CMS_SIGPOLICY_EXPLICITTEXT,
		/* sigPolicyQualifiers.sigPolicyQualifier.userNotice.explicitText */

	/* 1 2 840 113549 1 9 16 9 signatureTypeIdentifier */
	CRYPT_CERTINFO_CMS_SIGTYPEIDENTIFIER,
	CRYPT_CERTINFO_CMS_SIGTYPEID_ORIGINATORSIG, 	/* originatorSig */
	CRYPT_CERTINFO_CMS_SIGTYPEID_DOMAINSIG,		/* domainSig */
	CRYPT_CERTINFO_CMS_SIGTYPEID_ADDITIONALATTRIBUTES, /* additionalAttributesSig */
	CRYPT_CERTINFO_CMS_SIGTYPEID_REVIEWSIG,		/* reviewSig */

	/* 1 2 840 113549 1 9 25 3 randomNonce */
	CRYPT_CERTINFO_CMS_NONCE,			/* randomNonce */

	/* SCEP attributes:
	   2 16 840 1 113733 1 9 2 messageType
	   2 16 840 1 113733 1 9 3 pkiStatus
	   2 16 840 1 113733 1 9 4 failInfo
	   2 16 840 1 113733 1 9 5 senderNonce
	   2 16 840 1 113733 1 9 6 recipientNonce
	   2 16 840 1 113733 1 9 7 transID */
	CRYPT_CERTINFO_SCEP_MESSAGETYPE,		/* messageType */
	CRYPT_CERTINFO_SCEP_PKISTATUS,			/* pkiStatus */
	CRYPT_CERTINFO_SCEP_FAILINFO,			/* failInfo */
	CRYPT_CERTINFO_SCEP_SENDERNONCE,		/* senderNonce */
	CRYPT_CERTINFO_SCEP_RECIPIENTNONCE,		/* recipientNonce */
	CRYPT_CERTINFO_SCEP_TRANSACTIONID,		/* transID */

	/* 1 3 6 1 4 1 311 2 1 10 spcAgencyInfo */
	CRYPT_CERTINFO_CMS_SPCAGENCYINFO,
	CRYPT_CERTINFO_CMS_SPCAGENCYURL,		/* spcAgencyInfo.url */

	/* 1 3 6 1 4 1 311 2 1 11 spcStatementType */
	CRYPT_CERTINFO_CMS_SPCSTATEMENTTYPE,
	CRYPT_CERTINFO_CMS_SPCSTMT_INDIVIDUALCODESIGNING,	/* individualCodeSigning */
	CRYPT_CERTINFO_CMS_SPCSTMT_COMMERCIALCODESIGNING,	/* commercialCodeSigning */

	/* 1 3 6 1 4 1 311 2 1 12 spcOpusInfo */
	CRYPT_CERTINFO_CMS_SPCOPUSINFO,
	CRYPT_CERTINFO_CMS_SPCOPUSINFO_NAME,		/* spcOpusInfo.name */
	CRYPT_CERTINFO_CMS_SPCOPUSINFO_URL,		/* spcOpusInfo.url */

	/* Used internally */
	CRYPT_CERTINFO_LAST,

	/*********************/
	/* Keyset attributes */
	/*********************/
	CRYPT_KEYINFO_FIRST = 3000,

	CRYPT_KEYINFO_QUERY,				/* Keyset query */
	CRYPT_KEYINFO_QUERY_REQUESTS,			/* Query of requests in cert store */

	/* Used internally */
	CRYPT_KEYINFO_LAST,

	/*********************/
	/* Device attributes */
	/*********************/
	CRYPT_DEVINFO_FIRST = 4000,

	CRYPT_DEVINFO_INITIALISE,			/* Initialise device for use */
	CRYPT_DEVINFO_INITIALIZE = CRYPT_DEVINFO_INITIALISE,
	CRYPT_DEVINFO_AUTHENT_USER,			/* Authenticate user to device */
	CRYPT_DEVINFO_AUTHENT_SUPERVISOR,		/* Authenticate supervisor to dev.*/
	CRYPT_DEVINFO_SET_AUTHENT_USER,			/* Set user authent.value */
	CRYPT_DEVINFO_SET_AUTHENT_SUPERVISOR,		/* Set supervisor auth.val.*/
	CRYPT_DEVINFO_ZEROISE,				/* Zeroise device */
	CRYPT_DEVINFO_ZEROIZE = CRYPT_DEVINFO_ZEROISE,
	CRYPT_DEVINFO_LOGGEDIN,				/* Whether user is logged in */
	CRYPT_DEVINFO_LABEL,				/* Device/token label */

	/* Used internally */
	CRYPT_DEVINFO_LAST,

	/***********************/
	/* Envelope attributes */
	/***********************/
	CRYPT_ENVINFO_FIRST = 5000,

	/* Pseudo-information on an envelope or meta-information which is used to
	   control the way that data in an envelope is processed */
	CRYPT_ENVINFO_DATASIZE,			/* Data size information */
	CRYPT_ENVINFO_COMPRESSION,		/* Compression information */
	CRYPT_ENVINFO_CONTENTTYPE,		/* Inner CMS content type */
	CRYPT_ENVINFO_DETACHEDSIGNATURE,	/* Generate CMS detached signature */
	CRYPT_ENVINFO_SIGNATURE_RESULT,		/* Signature check result */
	CRYPT_ENVINFO_MAC,			/* Use MAC instead of encrypting */

	/* Resources required for enveloping/deenveloping */
	CRYPT_ENVINFO_PASSWORD,			/* User password */
	CRYPT_ENVINFO_KEY,			/* Conventional encryption key */
	CRYPT_ENVINFO_SIGNATURE,		/* Signature/signature check key */
	CRYPT_ENVINFO_SIGNATURE_EXTRADATA,	/* Extra information added to CMS sigs */
	CRYPT_ENVINFO_RECIPIENT,		/* Recipient email address */
	CRYPT_ENVINFO_PUBLICKEY,		/* PKC encryption key */
	CRYPT_ENVINFO_PRIVATEKEY,		/* PKC decryption key */
	CRYPT_ENVINFO_PRIVATEKEY_LABEL,		/* Label of PKC decryption key */
	CRYPT_ENVINFO_ORIGINATOR,		/* Originator info/key */
	CRYPT_ENVINFO_SESSIONKEY,		/* Session key */
	CRYPT_ENVINFO_HASH,			/* Hash value */
	CRYPT_ENVINFO_TIMESTAMP,		/* Timestamp information */

	/* Keysets used to retrieve keys needed for enveloping/deenveloping */
	CRYPT_ENVINFO_KEYSET_SIGCHECK,		/* Signature check keyset */
	CRYPT_ENVINFO_KEYSET_ENCRYPT,		/* PKC encryption keyset */
	CRYPT_ENVINFO_KEYSET_DECRYPT,		/* PKC decryption keyset */

	/* Used internally */
	CRYPT_ENVINFO_LAST,

	/**********************/
	/* Session attributes */
	/**********************/
	CRYPT_SESSINFO_FIRST = 6000,

	/* Pseudo-information on a session or meta-information which is used to
	   control the way that a session is managed */

	/* Pseudo-information about the session */
	CRYPT_SESSINFO_ACTIVE,			/* Whether session is active */
	CRYPT_SESSINFO_CONNECTIONACTIVE,	/* Whether network connection is active */

	/* Security-related information */
	CRYPT_SESSINFO_USERNAME,		/* User name */
	CRYPT_SESSINFO_PASSWORD,		/* Password */
	CRYPT_SESSINFO_PRIVATEKEY,		/* Server/client private key */
	CRYPT_SESSINFO_KEYSET,			/* Certificate store */
	CRYPT_SESSINFO_AUTHRESPONSE,		/* Session authorisation OK */

	/* Client/server information */
	CRYPT_SESSINFO_SERVER_NAME,		/* Server name */
	CRYPT_SESSINFO_SERVER_PORT,		/* Server port number */
	CRYPT_SESSINFO_SERVER_FINGERPRINT,	/* Server key fingerprint */
	CRYPT_SESSINFO_CLIENT_NAME,		/* Client name */
	CRYPT_SESSINFO_CLIENT_PORT,		/* Client port number */
	CRYPT_SESSINFO_SESSION,			/* Transport mechanism */
	CRYPT_SESSINFO_NETWORKSOCKET,		/* User-supplied network socket */

	/* Generic protocol-related information */
	CRYPT_SESSINFO_VERSION,			/* Protocol version */
	CRYPT_SESSINFO_REQUEST,			/* Cert.request object */
	CRYPT_SESSINFO_RESPONSE,		/* Cert.response object */
	CRYPT_SESSINFO_CACERTIFICATE,		/* Issuing CA certificate */

	/* Protocol-specific information */
	CRYPT_SESSINFO_TSP_MSGIMPRINT,		/* TSP message imprint */
	CRYPT_SESSINFO_CMP_REQUESTTYPE,		/* Request type */
	CRYPT_SESSINFO_CMP_PKIBOOT,		/* Enable PKIBoot facility */
	CRYPT_SESSINFO_CMP_PRIVKEYSET,		/* Private-key keyset */
	CRYPT_SESSINFO_SSH_CHANNEL,		/* SSH current channel */
	CRYPT_SESSINFO_SSH_CHANNEL_TYPE,	/* SSH channel type */
	CRYPT_SESSINFO_SSH_CHANNEL_ARG1,	/* SSH channel argument 1 */
	CRYPT_SESSINFO_SSH_CHANNEL_ARG2,	/* SSH channel argument 2 */
	CRYPT_SESSINFO_SSH_CHANNEL_ACTIVE,	/* SSH channel active */

	/* Used internally */
	CRYPT_SESSINFO_LAST, CRYPT_USERINFO_FIRST = 7000,

	/**********************/
	/* User attributes */
	/**********************/

	/* Security-related information */
	CRYPT_USERINFO_PASSWORD,		/* Password */

	/* User role-related information */
	CRYPT_USERINFO_CAKEY_CERTSIGN,		/* CA cert signing key */
	CRYPT_USERINFO_CAKEY_CRLSIGN,		/* CA CRL signing key */
	CRYPT_USERINFO_CAKEY_RTCSSIGN,		/* CA RTCS signing key */
	CRYPT_USERINFO_CAKEY_OCSPSIGN,		/* CA OCSP signing key */

	/* Used internally for range checking */
	CRYPT_USERINFO_LAST,
	CRYPT_ATTRIBUTE_LAST = CRYPT_USERINFO_LAST,

	/***********************/
	/* Internal attributes */
	/***********************/

	/* The following attributes are only visible internally and are protected
	   from any external access by the kernel (and for good measure by checks
	   in other places as well).  The two attributes CRYPT_IATTRIBUTE_KEY_SPKI
	   and CRYPT_IATTRIBUTE_SPKI are actually the same thing, the difference
	   is that the former is write-only for contexts and the latter is read-
	   only for certificates (the former is used when loading a context from
	   a key contained in a device, where the actual key components aren't
	   directly available in the context but may be needed in the future for
	   things like cert requests).  Because a single object can act as both a
	   context and a cert, having two explicitly different attribute names
	   makes things less confusing.  In addition, some public-key attributes
	   have _PARTIAL variants that load the public-key components but don't
	   initialise the key/move the context into the high state.  This is
	   used for formats in which public and private-key components are loaded
	   separately */
	CRYPT_IATTRIBUTE_FIRST = 8000,
	CRYPT_IATTRIBUTE_TYPE,			/* Object type */
	CRYPT_IATTRIBUTE_SUBTYPE,		/* Object subtype */
	CRYPT_IATTRIBUTE_STATUS,		/* Object status */
	CRYPT_IATTRIBUTE_INTERNAL,		/* Object internal flag */
	CRYPT_IATTRIBUTE_ACTIONPERMS,		/* Object action permissions */
	CRYPT_IATTRIBUTE_LOCKED,		/* Object locked for exclusive use */
	CRYPT_IATTRIBUTE_INITIALISED,		/* Object inited (in high state) */
	CRYPT_IATTRIBUTE_KEYSIZE,		/* Ctx: Key size (written to non-native ctxs) */
	CRYPT_IATTRIBUTE_KEYFEATURES,		/* Ctx: Key feature info */
	CRYPT_IATTRIBUTE_KEYID,			/* Ctx: Key ID */
	CRYPT_IATTRIBUTE_KEYID_PGP,		/* Ctx: PGP key ID */
	CRYPT_IATTRIBUTE_KEYID_OPENPGP,		/* Ctx: OpenPGP key ID */
	CRYPT_IATTRIBUTE_KEY_KEADOMAINPARAMS,	/* Ctx: Key agreement domain parameters */
	CRYPT_IATTRIBUTE_KEY_KEAPUBLICVALUE,	/* Ctx: Key agreement public value */
	CRYPT_IATTRIBUTE_KEY_SPKI,		/* Ctx: SubjectPublicKeyInfo */
	CRYPT_IATTRIBUTE_KEY_PGP,		/* Ctx: PGP-format public key */
	CRYPT_IATTRIBUTE_KEY_SSH1,		/* Ctx: SSHv1-format public key */
	CRYPT_IATTRIBUTE_KEY_SSH2,		/* Ctx: SSHv2-format public key */
	CRYPT_IATTRIBUTE_KEY_SSL,		/* Ctx: SSL-format public key */
	CRYPT_IATTRIBUTE_KEY_SPKI_PARTIAL,	/* Ctx: SubjectPublicKeyInfo w/o trigger */
	CRYPT_IATTRIBUTE_KEY_PGP_PARTIAL,	/* Ctx: PGP public key w/o trigger */
	CRYPT_IATTRIBUTE_PGPVALIDITY,		/* Ctx: PGP key validity */
	CRYPT_IATTRIBUTE_DEVICEOBJECT,		/* Ctx: Device object handle */
	CRYPT_IATTRIBUTE_CRLENTRY,		/* Cert: Individual entry from CRL */
	CRYPT_IATTRIBUTE_SUBJECT,		/* Cert: SubjectName */
	CRYPT_IATTRIBUTE_ISSUER,		/* Cert: IssuerName */
	CRYPT_IATTRIBUTE_ISSUERANDSERIALNUMBER,	/* Cert: IssuerAndSerial */
	CRYPT_IATTRIBUTE_SPKI,			/* Cert: Encoded SubjectPublicKeyInfo */
	CRYPT_IATTRIBUTE_CERTCOLLECTION,	/* Cert: Certs added to cert chain */
	CRYPT_IATTRIBUTE_RESPONDERURL,		/* Cert: RTCS/OCSP responder name */
	CRYPT_IATTRIBUTE_RTCSREQUEST,		/* Cert: RTCS req.info added to RTCS resp.*/
	CRYPT_IATTRIBUTE_OCSPREQUEST,		/* Cert: OCSP req.info added to OCSP resp.*/
	CRYPT_IATTRIBUTE_REVREQUEST,		/* Cert: CRMF rev.request added to CRL */
	CRYPT_IATTRIBUTE_PKIUSERINFO,		/* Cert: Additional user info added to cert.req.*/
	CRYPT_IATTRIBUTE_BLOCKEDATTRS,		/* Cert: Template of disallowed attrs.in cert */
	CRYPT_IATTRIBUTE_AUTHCERTID,		/* Cert: Authorising cert ID for a cert/rev.req.*/
	CRYPT_IATTRIBUTE_ESSCERTID,		/* Cert: ESSCertID */
	CRYPT_IATTRIBUTE_ENTROPY,		/* Dev: Polled entropy data */
	CRYPT_IATTRIBUTE_ENTROPY_QUALITY,	/* Dev: Quality of entropy data */
	CRYPT_IATTRIBUTE_RANDOM_LOPICKET,	/* Dev: Low picket for random data attrs.*/
	CRYPT_IATTRIBUTE_RANDOM,		/* Dev: Random data */
	CRYPT_IATTRIBUTE_RANDOM_NZ,		/* Dev: Nonzero random data */
	CRYPT_IATTRIBUTE_RANDOM_HIPICKET,	/* Dev: High picket for random data attrs.*/
	CRYPT_IATTRIBUTE_RANDOM_NONCE,		/* Dev: Basic nonce */
	CRYPT_IATTRIBUTE_SELFTEST,		/* Dev: Perform self-test */
	CRYPT_IATTRIBUTE_TIME,			/* Dev: Reliable (hardware-based) time value */
	CRYPT_IATTRIBUTE_INCLUDESIGCERT,	/* Env: Whether to include signing cert(s) */
	CRYPT_IATTRIBUTE_ATTRONLY,		/* Env: Signed data contains only CMS attrs.*/
	CRYPT_IATTRIBUTE_CONFIGDATA,		/* Keyset: Config information */
	CRYPT_IATTRIBUTE_USERINDEX,		/* Keyset: Index of users */
	CRYPT_IATTRIBUTE_USERID,		/* Keyset: User ID */
	CRYPT_IATTRIBUTE_USERINFO,		/* Keyset: User information */
	CRYPT_IATTRIBUTE_TRUSTEDCERT,		/* Keyset: First trusted cert */
	CRYPT_IATTRIBUTE_TRUSTEDCERT_NEXT,	/* Keyset: Successive trusted certs */
	CRYPT_IATTRIBUTE_ENC_TIMESTAMP,		/* Session: Encoded TSA timestamp */
	CRYPT_IATTRUBUTE_CERTKEYSET,		/* User: Keyset to send trusted certs to */
	CRYPT_IATTRIBUTE_CTL,			/* User: Cert.trust list */
	CRYPT_IATTRIBUTE_CERT_TRUSTED,		/* User: Set trusted cert */
	CRYPT_IATTRIBUTE_CERT_UNTRUSTED,	/* User: Unset trusted cert */
	CRYPT_IATTRIBUTE_CERT_CHECKTRUST,	/* User: Check trust status of cert */
	CRYPT_IATTRIBUTE_CERT_TRUSTEDISSUER,	/* User: Get trusted issuer of cert */
	CRYPT_IATTRIBUTE_LAST,

	/* Subrange values used internally for range checking */
	CRYPT_CERTINFO_FIRST_CERTINFO    = CRYPT_CERTINFO_FIRST + 1,
	CRYPT_CERTINFO_LAST_CERTINFO     = CRYPT_CERTINFO_PKIUSER_REVPASSWORD,
	CRYPT_CERTINFO_FIRST_PSEUDOINFO  = CRYPT_CERTINFO_SELFSIGNED,
	CRYPT_CERTINFO_LAST_PSEUDOINFO   = CRYPT_CERTINFO_SIGNATURELEVEL,
	CRYPT_CERTINFO_FIRST_NAME        = CRYPT_CERTINFO_COUNTRYNAME,
	CRYPT_CERTINFO_LAST_NAME         = CRYPT_CERTINFO_REGISTEREDID,
	CRYPT_CERTINFO_FIRST_DN          = CRYPT_CERTINFO_COUNTRYNAME,
	CRYPT_CERTINFO_LAST_DN           = CRYPT_CERTINFO_COMMONNAME,
	CRYPT_CERTINFO_FIRST_GENERALNAME = CRYPT_CERTINFO_OTHERNAME_TYPEID,
	CRYPT_CERTINFO_LAST_GENERALNAME  = CRYPT_CERTINFO_REGISTEREDID,
	CRYPT_CERTINFO_FIRST_EXTENSION   = CRYPT_CERTINFO_CHALLENGEPASSWORD,
	CRYPT_CERTINFO_LAST_EXTENSION    = CRYPT_CERTINFO_SET_TUNNELINGALGID,
	CRYPT_CERTINFO_FIRST_CMS         = CRYPT_CERTINFO_CMS_CONTENTTYPE,
	CRYPT_CERTINFO_LAST_CMS          = CRYPT_CERTINFO_LAST - 1,
	CRYPT_SESSINFO_FIRST_SPECIFIC    = CRYPT_SESSINFO_REQUEST,
	CRYPT_SESSINFO_LAST_SPECIFIC     = CRYPT_SESSINFO_SSH_CHANNEL_ACTIVE
} CRYPT_ATTRIBUTE_TYPE;

/* Flags for the X.509 keyUsage extension */

#define CRYPT_KEYUSAGE_NONE			0x000
#define CRYPT_KEYUSAGE_DIGITALSIGNATURE		0x001
#define CRYPT_KEYUSAGE_NONREPUDIATION		0x002
#define CRYPT_KEYUSAGE_KEYENCIPHERMENT		0x004
#define CRYPT_KEYUSAGE_DATAENCIPHERMENT		0x008
#define CRYPT_KEYUSAGE_KEYAGREEMENT		0x010
#define CRYPT_KEYUSAGE_KEYCERTSIGN		0x020
#define CRYPT_KEYUSAGE_CRLSIGN			0x040
#define CRYPT_KEYUSAGE_ENCIPHERONLY		0x080
#define CRYPT_KEYUSAGE_DECIPHERONLY		0x100
#define CRYPT_KEYUSAGE_LAST			0x200	/* Last possible value */


/* OCSP revocation status */

enum {
	CRYPT_OCSPSTATUS_NOTREVOKED,
	CRYPT_OCSPSTATUS_REVOKED,
	CRYPT_OCSPSTATUS_UNKNOWN
};


#define X509_FREE_BIN(bin) \
	if ((bin)->data && (bin)->len > 0) {	\
		aos_free((bin)->data;	\
		(bin)->data = NULL;	\
		(bin)->len = 0;		\
	}

typedef struct {
	BYTE	C[X509_MAX_TEXTSIZE];
	int	C_len;
	int	C_tag;
	BYTE	S[X509_MAX_TEXTSIZE*2];
	int	S_len;
	int	S_tag;
	BYTE	L[X509_MAX_TEXTSIZE*2];
	int	L_len;
	int	L_tag;
	BYTE	O[X509_MAX_TEXTSIZE];
	int	O_len;
	int	O_tag;
	BYTE	OU[X509_MAX_TEXTSIZE];
	int	OU_len;
	int	OU_tag;
	BYTE	CN[X509_MAX_TEXTSIZE];
	int	CN_len;
	int	CN_tag;
	BYTE	E[X509_MAX_TEXTSIZE];
	int	E_len;
	int	E_tag;
	BYTE	hash[SHA1_HASH_SIZE]; //for OCSP Request
} DN_attributes_t;

typedef struct {
	int	ca;
	int	path_len_constraint;
} ext_basic_constraints_t;

typedef struct {
	BYTE	*		dns;
	BYTE	*		uri;
	BYTE	*		email;
} ext_subject_altname_t;

typedef asn_bin_t ext_subject_key_id_t;

typedef struct {
	int			key_len;
	BYTE	*		key_id;
	DN_attributes_t		attribs;
	int			serial_num;
} ext_auth_key_id_t;
/*
	FUTURE:  add support for the other extensions
*/
typedef struct {
	ext_basic_constraints_t	bc;
	ext_subject_altname_t	san;
	ext_subject_key_id_t	sk;
	ext_auth_key_id_t	ak;
	BYTE			key_usage;
} v3extensions_t;

typedef struct {
	int			algorithm;
	int			sub_algorithm;
}	x509_algorithm_t;

typedef struct rsa_pubkey {
	x509_algorithm_t	alg;
	asn_bin_t		modulus;
	asn_bin_t		public_exponent;
	BYTE			hash[SHA1_HASH_SIZE];//for OCSP Request
}	rsa_pubkey_t;

typedef struct {
	DATA_OBJECT  *		object;

	long			version;
	asn_bin_t		modulus;
	asn_bin_t		public_exponent;
	asn_bin_t		private_exponent;
	asn_bin_t		prime1;
	asn_bin_t		prime2;
	asn_bin_t		exponent1;
	asn_bin_t		exponent2;
	asn_bin_t		coefficient;
}	rsa_privatekey_t;


typedef struct {
	time_t			notbefore;
	time_t			notafter;
}	x509_validity_t;


enum
{
	eAosCertNameLen = 16,
};

struct AosRsaPrivateKey;
// 
// Chen Ding, 09/16/2005, Changed from "__X509" to "_X509_CERT_INFO"
typedef struct _X509_CERT_INFO 
{
	struct aos_list_head 	*next;
	struct aos_list_head 	*prev;

	// DATA_OBJECT	*       object;
	char			name[eAosCertNameLen];	
	int			valid;
	int			version;
	asn_data_t		serial_number;
//	char			police_id[USBKEYLEN+1];
	x509_algorithm_t	sig_alg;
	DN_attributes_t		issuer;
	x509_validity_t		validity;
	DN_attributes_t		subject;
	rsa_pubkey_t		pubkey;
	int			sig_algorithm;
	int			pubkey_algorithm;

	asn_bin_t		subject_unique_id;
	asn_bin_t		issuer_unique_id;

	v3extensions_t		extensions;

	x509_algorithm_t	signature_algorithm;
	asn_bin_t		signature;
	char *			cert_der;
	u16			cert_der_len;
	//char *		cert_base64;
	atomic_t		refcnt;
	char *			issuer_der;			// The DER of the issuer
	u16			issuer_der_len; 	// by liqin	
	char *			subject_der;		// The DER of the issuer
	u16			subject_der_len; 	// by liqin	
	struct AosRsaPrivateKey *prikey;// by liqin

	BYTE			hash[SHA1_HASH_SIZE];
} X509_CERT_INFO;


typedef struct __crl_entry {
	asn_data_t		serial_number;
	time_t			revocation_date;
				/* skip crlEntry extensions */
	struct __crl_entry *	next;
}	X509_CRL_ENTRY;

typedef struct __CRL_INFO {
	DATA_OBJECT *		object;

	int			version;

	DN_attributes_t		issuer;
	time_t			this_update;
	time_t			next_update;

	X509_CRL_ENTRY	 *	list;
	int			entry_num;

	x509_algorithm_t	sig_alg;
	asn_bin_t		signature;

	BYTE * 			certlist_data_ptr;
	int 			certlist_data_len;

}	X509_CRL_INFO;



/* certificate routine */
struct AosRsaPrivateKey;
extern int AosCert_decodePrivKey(
			const char *priv_key_base64, 
			struct AosRsaPrivateKey *priv_key);
extern int AosCert_constructor(X509_CERT_INFO **cert);
extern int x509_cert_destructor(X509_CERT_INFO *cert);
extern int x509_cert2str(X509_CERT_INFO *cert, char *buffer, u32 buflen);
extern int x509_decode_serial_number(DATA_OBJECT *object, asn_data_t *sn, const int tag);
extern int x509_decode_algo_id(DATA_OBJECT *object, x509_algorithm_t *alg, int *extra_length, const int tag);
extern int x509_read_DN(DATA_OBJECT *object, DN_attributes_t *dn);
extern CRYPT_ALGO_TYPE x509_oid2algorithm(const BYTE *oid, int *sub_algorithm);
extern const BYTE * x509_algorithm2oid(const CRYPT_ALGO_TYPE algorithm, const CRYPT_ALGO_TYPE sub_algorithm);
extern int x509_decode_signature(DATA_OBJECT *object, asn_bin_t *signature);
extern int x509_decode_certinfo(const char *cert_der, u32 cert_len, X509_CERT_INFO *certinfo);
extern int x509_decode_certinfo_base64(const char *cert_base64, X509_CERT_INFO *certinfo);
extern X509_CERT_INFO * x509_parse_cert_object(DATA_OBJECT *object);
static inline void x509_free_certinfo(X509_CERT_INFO *certinfo) {
	X509_CERT_INFO * tmp;

	while(certinfo) {
		tmp = certinfo;
		certinfo = (X509_CERT_INFO*)certinfo->next;

	//	if (tmp->object)
	//		asn_free_object(tmp->object);
		aos_free(tmp);
	}
}


/* decode private key routine */
extern int x509_decode_private_key_info(DATA_OBJECT *object, rsa_privatekey_t *prvkey);
static inline void x509_free_prvkey(rsa_privatekey_t *prvkey) {
	if (prvkey->object)
		asn_free_object(prvkey->object);

	aos_free(prvkey);
}

/* CRL routine */
extern int x509_decode_crlinfo(DATA_OBJECT *object, X509_CRL_INFO *crlinfo);
static inline void x509_free_crlinfo(X509_CRL_INFO *crlinfo) {
	if (crlinfo->object)
		asn_free_object(crlinfo->object);

	if (crlinfo->entry_num > 0) {
		while (crlinfo->list) {
			X509_CRL_ENTRY	*entry = crlinfo->list;

			crlinfo->list = crlinfo->list->next;
			aos_free(entry);
			crlinfo->entry_num--;
		}
	}

	aos_free(crlinfo);
}



#define x509_debug(object) \
	do	{	\
		int i;	\
			\
		printf("[%s][%s](%d)---next to read: ",__FILE__,__FUNCTION__,__LINE__);	\
		for(i=0;i <10; i++) printf("%02x ", (object)->data[(object)->pos + i]);	\
		printf("\n");\
	} while(0)

#define x509_ret(object, ret) \
	do {	\
		int i;\
			\
		printf("[%s][%s](%d)---next to read: ",__FILE__,__FUNCTION__,__LINE__);\
		for(i=0;i <10; i++) printf("%02x ", (object)->data[(object)->pos + i]);\
		printf("ret:%d\n", ret);\
	}	while(0);

static inline int AosCert_put(X509_CERT_INFO *cert)
{
	if (atomic_dec_and_test(&(cert->refcnt)))
	{
		// 
		// No one is using the certificate. Delete it.
		//
		x509_cert_destructor(cert);
	}

	return 0;
}

static inline int AosCert_hold(X509_CERT_INFO *cert)
{
	atomic_inc(&cert->refcnt);
	return 0;
}

static inline char *AosCert_getIssuerDer(X509_CERT_INFO *cert)
{
	return cert->issuer_der;
}


#endif //endof __X509__

