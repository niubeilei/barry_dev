////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiI_h
#define AOS_API_ApiI_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "UserMgmt/Ptrs.h"
#include "XmlUtil/Ptrs.h"

extern bool					AosIsAccessRecord(const AosXmlTagPtr &access_record);

extern bool					AosIsAdmin( 		const AosUserAcctObjPtr &requester_acct,
												const AosUserDomainObjPtr &requester_domain, 
												const AosUserDomainObjPtr &accessed_domain);

extern bool					AosIsRoot( 			const AosXmlTagPtr &requester_acct,
												const AosUserDomainObjPtr &requester_domain, 
												const AosUserDomainObjPtr &accessed_domain);

extern bool					AosIsGuestUser(const AosUserAcctObjPtr &doc);

extern bool					AosIsSupervisor(
												const AosUserAcctObjPtr &accessed_acct, 
												const AosUserAcctObjPtr &supervisor_acct, 
												AosRundata *rdata);

extern bool					AosIsUnknownUser(	const AosUserAcctObjPtr &acct);

extern bool					AosIsUserAccount(	const AosXmlTagPtr &doc);

inline bool AosIncrementKeyedValue(
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->incrementDocid(iilname, key, value, 
			persis, incValue, initValue, add_flag, rdata);
}


inline bool AosIncrementKeyedValue(
		const OmnString &iilname,
		const u64 &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dft_value,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->incrementDocid(iilname, key, value, 
			persis, incValue, initValue, add_flag, dft_value, rdata);
}


inline bool AosIncrementKeyedValue(
		const u64 &iilid,
		const OmnString &key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->incrementDocid(iilid, key, value, incValue, initValue, add_flag, rdata);
}

inline bool AosIncrementDocid(
		const u64 &iilid,
		const u64 &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dftValue,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->incrementDocid(iilid, key, value, persis,
			incValue, initValue, add_flag, dftValue, rdata);
}

inline bool AosIsIILLocal(const OmnString &iilname)
{
	// Ketty 2013/07/13
	OmnNotImplementedYet;
	return false;
	//return AosNetworkMgrObj::isIILLocal(iilname);
}


extern bool AosIsVirtualIdLocal(const int virtual_id);
extern bool AosIsPhysicalIdLocal(const int physical_id);

extern bool AosIsFileLocal(const OmnString &fname);

// Ketty 2013/07/15
//extern bool AosIsNumPhysicalsValid(const int num_physicals);
extern bool AosIsValidPhysicalIdNorm(const int physical_id);
extern bool AosIsSelfCubeSvr();
extern bool AosIsConfigCubeMaster();

// Chen Ding, 2014/01/29
extern bool AosIsParalIIL(AosRundata *rdata, const OmnString &iilname);

extern bool AosIsCellPhoneNum(const OmnString &num);

extern bool AosIsRaidFile(const u64 &fileId);

extern bool AosIsSuperIIL(const OmnString &iilname);

extern bool AosIILRebuildBitmap(const OmnString &iilname, const AosRundataPtr &rdata);

#endif

