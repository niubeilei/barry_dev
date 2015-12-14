////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CRL.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/CRL.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "PKCS/x509.h"
#include "PKCS/x509_object.h"
#include "PKCS/asn1.h"
#include "PKCS/ReturnCode.h"

static int
x509_decode_crl_entry(DATA_OBJECT *object, X509_CRL_ENTRY **list) 
{
	X509_CRL_ENTRY * crl_entry = NULL;
	int length, ret;
	u32 end_pos;

	aos_assert1(object && list);

	/* decode wraper of sequence */
	if ((ret = asn_decode_sequence(object, &length)))
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode CRL: %d", ret);
		goto crl_entry_failed;
	}

	crl_entry = (X509_CRL_ENTRY *)aos_malloc(sizeof(X509_CRL_ENTRY));
	aos_assert1(crl_entry);
	memset(crl_entry, 0, sizeof(X509_CRL_ENTRY));
	end_pos = asn_object_pos(object) + length;

	//
	// userCertificate    CertificateSerialNumber 
	//
	if ((ret = x509_decode_serial_number(object, 
		&crl_entry->serial_number, DEFAULT_TAG)))
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode certificate serial number: %d", ret);
		goto crl_entry_failed;
	}	

	//
	// revocationDate     Time 
	//
	if (asn_peek_tag(object) == BER_TIME_UTC)
	{
		if ((ret = asn_decode_UTC_time(object, &crl_entry->revocation_date)))
		{
			aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
				"Failed to decode TIME_UTC: %d", ret);
			goto crl_entry_failed;
		}	
	}
	else if (asn_peek_tag(object) == BER_TIME_GENERALIZED)
	{
		if ((ret = asn_decode_generalized_time(object, 
			&crl_entry->revocation_date)))
		{
			aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
				"Failed to decode TIME_GENERALIZED: %d", ret);
			goto crl_entry_failed;
		}	
	}

	//
	// crlEntryExtensions OPTIONAL , if exist, skip it 
	//
	if (asn_object_pos(object) < (end_pos - MIN_ATTRIBUTE_SIZE))
	{
		if ((ret = asn_decode_universal(object)))
		{
			aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
				"Failed to decode universal: %d", ret);
			goto crl_entry_failed;
		}
	}

	/* add crl_entry to list head */
	crl_entry->next = *list;
	*list = crl_entry;

	return 0;

crl_entry_failed:
	if (crl_entry)	aos_free(crl_entry);
	return -1;
}


/*
TBSCertList  ::=  SEQUENCE  {
	version                 Version OPTIONAL,
		                     -- if present, MUST be v2
	signature               AlgorithmIdentifier,
	issuer                  Name,
	thisUpdate              Time,
	nextUpdate              Time OPTIONAL,
	revokedCertificates     SEQUENCE OF SEQUENCE  {
					userCertificate         CertificateSerialNumber,
					revocationDate          Time,
					crlEntryExtensions      Extensions OPTIONAL
					                           -- if present, MUST be v2
	}  OPTIONAL,
	crlExtensions           [0]  EXPLICIT Extensions OPTIONAL
	                           -- if present, MUST be v2
}
*/

static int
x509_decode_crl_certlist(DATA_OBJECT *object, X509_CRL_INFO *crlinfo) 
{
	long length, end_pos, ret = 0;

	//
	// decode wraper of sequence 
	//
	ret = asn_decode_long_sequence(object, &length);
	if (ret < 0) 	return -1;
	end_pos = asn_object_pos(object) + length;

	/* version */
	if (asn_peek_tag(object) == BER_INTEGER) {
		long version;

		ret = asn_decode_short_integer(object, &version);
		crlinfo->version = version + 1;	/* Zero-based */
	} else
		crlinfo->version = 1;	/* default */

	/* signature     AlgorithmIdentifier */
	if (ret == 0)
		ret = x509_decode_algo_id(object, &crlinfo->sig_alg, NULL, DEFAULT_TAG);

	/* issuer */
	if (ret == 0)
		ret = x509_read_DN(object, &crlinfo->issuer);

	/* this_Update */
	if (ret == 0) {
		if (asn_peek_tag(object) == BER_TIME_UTC)
			ret = asn_decode_UTC_time(object, &crlinfo->this_update);
		else if (asn_peek_tag(object) == BER_TIME_GENERALIZED)
			ret = asn_decode_generalized_time(object, &crlinfo->this_update);
		else
			return -1;
	}

	/* nextUpdate */
	if (ret == 0) {
		if (asn_peek_tag(object) == BER_TIME_UTC)
			ret = asn_decode_UTC_time(object, &crlinfo->next_update);
		else if (asn_peek_tag(object) == BER_TIME_GENERALIZED)
			ret = asn_decode_generalized_time(object, &crlinfo->next_update);
	}

	if (ret < 0)
		return -1;
	/* revokedCertificates OPTIONAL */
	if (asn_object_pos(object) < (u32)end_pos - MIN_ATTRIBUTE_SIZE) {
		ret = asn_decode_long_sequence(object, (long *)&length);

		if (ret == 0 && length == ASN_UNUSED)
			ret = -1;

		crlinfo->entry_num = 0;
		while (ret == 0 && length > MIN_ATTRIBUTE_SIZE) {
			int inner_start_pos = asn_object_pos(object);

			ret = x509_decode_crl_entry(object, &crlinfo->list);
			if (ret == 0) {
				crlinfo->entry_num++;
				length -= asn_object_pos(object) - inner_start_pos;
			}
		}

		if (ret < 0)
			return ret;
	}

	/* crlExtensions */
	if (asn_object_pos(object) < (u32)end_pos - MIN_ATTRIBUTE_SIZE)
		ret = asn_decode_universal(object);

	return ret;
}


/*
	CertificateList  ::=  SEQUENCE  {
		tbsCertList          TBSCertList,
		signatureAlgorithm   AlgorithmIdentifier,
		signatureValue       BIT STRING
	}
*/


int
x509_decode_crlinfo(DATA_OBJECT *object, X509_CRL_INFO *crlinfo) 
{
	x509_algorithm_t alg;
	long length;
	int  ret;

	/* decode wraper of sequence */
	ret = asn_decode_long_sequence(object, &length);
	if (ret < 0)
		return -1;

	/* save Certlist pointer */
	crlinfo->certlist_data_ptr = asn_object_cur_ptr(object);

	/* TBSCertList */
	ret = x509_decode_crl_certlist(object, crlinfo);
	if (ret < 0)
		return -ret;

	/* save Certlist length */
	crlinfo->certlist_data_len = asn_object_cur_ptr(object) - crlinfo->certlist_data_ptr;

	/* signatureAlgorithm 	AlgorithmIdentifier */
	ret = x509_decode_algo_id(object, &alg, NULL, DEFAULT_TAG);
	if (ret < 0)
		return ret;

	if (alg.algorithm != crlinfo->sig_alg.algorithm ||
	    alg.sub_algorithm != crlinfo->sig_alg.sub_algorithm)
	    return -1;

	ret = x509_decode_signature(object, &crlinfo->signature);

	if (ret == 0)
		crlinfo->object = object;

	return ret;
}


// 
// Below is written by Chen Ding. We comment it out for now and use
// the one Zhang Yunqiao wrote. 
//
#if 0
static int aosCRL_constructor(struct aosCRL **crl)
{
	int ret;

	*crl = (struct aosCRL *)OmnKernelAlloc(sizeof(struct aosCRL));
	aosKernelAssert(crl, eAosRc_MemErr);

	(*crl)->version = 0;
	(*crl)->this_update[0] = 0;
	(*crl)->next_update[0] = 0;
	ret = AosHashInt_constructor(&(*crl)->hash_table, 4095, 50000);
	aosKernelAssert((*crl)->hash_table, eAosRc_MemErr);
	aosKernelAssert(ret == 0, ret);
	return 0;
}


static int aosCRL_reset(struct aosCRL *crl)
{
	crl->version = 0;
	crl->this_update[0] = 0;
	crl->next_update[0] = 0;
	AosHashInt_reset(crl->hash_table);
	return 0;
}


int aosCRL_addCert(struct aosCRL *crl, 
				   unsigned int sn)
{
	// 
	// It adds the entry into the hash table
	//
	return AosHashInt_add(crl->hash_table, sn, 0, 0);
}


int aosCRL_decode(char *buffer, 
				  unsigned int buflen,
				  struct aosCRL *crl)
{
	//
	//	CertificateList  ::=  SEQUENCE  {
    //		tbsCertList          TBSCertList,
	//		signatureAlgorithm   AlgorithmIdentifier,
	//		signatureValue       BIT STRING  }
	//
	//	TBSCertList  ::=  SEQUENCE  {
	//		version                 Version OPTIONAL,
	//                                -- if present, shall be v2
	//		signature               AlgorithmIdentifier,
	//		issuer                  Name,
	//		thisUpdate              Time,
	//		nextUpdate              Time OPTIONAL,
	//		revokedCertificates     SEQUENCE OF SEQUENCE  {
	//			userCertificate         CertificateSerialNumber,
	//			revocationDate          Time,
	//			crlEntryExtensions      Extensions OPTIONAL
	//		                           -- if present, shall be v2
	//      }  OPTIONAL,
	//		crlExtensions           [0]  EXPLICIT Extensions OPTIONAL
	//                               -- if present, shall be v2
	//	}

	int ret;
	unsigned int tbslen, crlFinish;
	unsigned int tagid;
	unsigned char theclass, primitive;
	unsigned int cursor;

	aosCRL_reset(crl);

	// 
	// Get the length
	//
	ret = aosAsn1Codec_getSeqLen(buffer, &cursor, &tbslen);
	aosKernelAssert(ret == 0, eAosRc_DecodeFailed);

	crlFinish = tbslen + cursor;

	//
	// Decode the version. 
	//
	if (buffer[cursor++] == eAosAsn_TagId_Integer)
	{
		buffer[cursor++];
		crl->version = buffer[cursor++];
	}

	//
	// Decode AlgorithmId
	//
	ret |= aosAsn1Codec_decodeAlgId(buffer, tbslen, &cursor, &crl->signature_alg);
	aosKernelAssert(ret == 0, eAosRc_DecodeFailed);

	// 
	// Decode Issuer
	//
	ret |= aosAsn1Codec_decodeDN(buffer, tbslen, &cursor, &crl->issuer);

	// 
	// Decode this_update
	//
	ret |= aosAsn1Codec_utctime_decode(&tagid, &theclass, &primitive, crl->this_update, 
		ePkcsUctTimeMaxLen, buffer, tbslen, &cursor);

	// 
	// Decode next_update. It is optional
	//
	if (buffer[cursor] == eAosAsn_TagId_UTCTime)
	{
		// 
		// There is next_update
		//
		ret |= aosAsn1Codec_utctime_decode(&tagid, &theclass, &primitive, crl->next_update, 
		ePkcsUctTimeMaxLen, buffer, tbslen, &cursor);
	}
	else
	{
		crl->next_update[0] = 0;
	}

	// 
	// Decode the list
	//
	if (buffer[cursor] == eAosAsn_TagId_Sequence)
	{
		// 
		// There is CRL list
		//
		unsigned int listlen;
		ret = aosAsn1Codec_getSeqLen(buffer, &cursor, &listlen);
		aosKernelAssert(ret == 0, eAosRc_DecodeFailed);
		unsigned int listfinish = listlen + cursor;

		while (cursor < listfinish)
		{
			unsigned int entrylen;
			unsigned int entryfinish;
			unsigned int extlen;
			ret = aosAsn1Codec_getSeqLen(buffer, &cursor, &entrylen);
			aosKernelAssert(ret == 0, eAosRc_DecodeFailed);
			entryfinish = entrylen + cursor;

			// 
			// Decode serial number
			//
			int sn;
			ret = aosAsn1Codec_integer_decode(buffer, entrylen + cursor, &cursor, &sn);
			aosKernelAssert(ret == 0, eAosRc_DecodeFailed);
			aosCRL_addCert(crl, sn);

			// 
			// Decode revocationTime. Currently, we simply pass over this field
			//
			cursor++;
			cursor += buffer[cursor];

			//
			// Decode entry extensions. Currently, we simply pass over this field
			//
			if (cursor < entryfinish)
			{
				// 
				// There are extensions
				//
				cursor++;
				ret = aosAsn1Codec_getLength(buffer, &cursor, &extlen);
				aosKernelAssert(ret == 0, ret);

				cursor += extlen;
			}
		}
	}

	// 
	// We don't care about the extensions for now
	//
	return 0;
}
#endif

