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
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApi.h"

#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "UserMgmt/UserDomain.h"
#include "UserMgmt/UserAcct.h"
#include "Util/String.h"


//extern bool gSvrIsUp;
bool AosIsAccessRecord(const AosXmlTagPtr &access_record)
{
	if (!access_record) return false;
	return access_record->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD;
}


/*
bool AosIsSupervisor(
		const AosUserAcctObjPtr &accessed_acct,
		const AosUserAcctObjPtr &supervisor_acct, 
		AosRundata *rdata)
{
	// This function checks whether 'supervisor_acct' is a supervisor of
	// 'accessed_acct'. 
	if (!accessed_acct || !supervisor_acct) return false;
	return accessed_acct->isSupervisor(supervisor_acct, rdata);
}
*/


bool AosIsUserAccount(const AosXmlTagPtr &user_account)
{
	// In the current implementations, we simply check its otype.
	aos_assert_r(user_account, false);
	return user_account->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT;
}


bool AosIsAdmin( 		
		const AosUserAcctObjPtr &requester_acct,
		const AosUserDomainObjPtr &requester_domain, 
		const AosUserDomainObjPtr &accessed_domain)
{
	if (!requester_acct) return false;
	return requester_acct->isAdmin(requester_domain, accessed_domain);
}


bool AosIsRoot( 			
		const AosXmlTagPtr &requester_acct,
		const AosUserDomainObjPtr &requester_domain, 
		const AosUserDomainObjPtr &accessed_domain)
{
	if (!requester_domain || !accessed_domain) return false;
	if (!requester_domain->isFriendDomain(accessed_domain)) return false;
	if (!requester_acct) return false;
	if (!AosIsUserAccount(requester_acct)) return false;
	return (requester_acct->getAttrStr(AOSTAG_USERTYPE) == AOSUSERTYPE_ROOT);
}


bool AosIsGuestUser(const AosUserAcctObjPtr &guest_acct)
{
	if (!guest_acct) return false;
	return guest_acct->isGuestUser();
}


bool AosIsUnknownUser(const AosUserAcctObjPtr &unknown_acct)
{
	if (!unknown_acct) return true;
	return unknown_acct->isUnknownUser();
}


bool AosIsUserAccount(	
		const AosXmlTagPtr &doc, 
		AosRundata *rdata)
{
	if (!doc) return false;
	if (doc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_USERACCT) return false;
	return true;
}


bool AosIsFileLocal(const OmnString &fname)
{
	// This function assumes it is a working file that is in the 
	// following format:
	// 	basedir/working/machine/fname
	int physical_id = AosGetPhysicalIdFromFilename(fname);
	int self_physical_id = AosGetSelfServerId();
	return physical_id == self_physical_id;
}


//bool AosIsNumPhysicalsValid(const int num_physicals)
//{
//	return AosNetworkMgrObj::isNumPhysicalsValid(num_physicals);	
//}


bool AosIsValidPhysicalIdNorm(const int physical_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->isValidSvrId(physical_id);	
}

bool AosIsSelfCubeSvr()
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	
	AosProcessType::E tp = obj->getSelfProcType();
	// Ketty 2013/11/04
	//return tp == AosProcessType::eCube || tp == AosProcessType::eBkpCube || tp == AosProcessType::eLocalServer;
	return tp == AosProcessType::eCube || tp == AosProcessType::eBkpCube;
}

bool AosIsConfigCubeMaster()
{
	aos_assert_r(AosIsSelfCubeSvr(), false);

	int config_master = AosGetSelfConfigMaster();
	aos_assert_r(config_master >=0, false);
	return config_master == AosGetSelfServerId();
}

bool AosIsPhysicalIdLocal(const int physical_id)
{
	return AosGetSelfServerId() == physical_id;
}

bool AosIsVirtualIdLocal(const int virtual_id)
{
	AosNetworkMgrObjPtr obj = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(obj, false);
	return obj->isVirtualIdLocal(virtual_id);
}


// Chen Ding, 2014/01/29
bool AosIsParalIIL(AosRundata *rdata, const OmnString &iilname)
{
	return AosIndexMgrObj::isParalIILStatic(rdata, iilname);
}


bool AosIsCellPhoneNum(const OmnString &num)
{
	int len = num.length();
	if (len < 11) return false;
	OmnString value(&(num.data()[len - 11]), 11);
	const char * data = value.data();
	if (data[0] != '1') return false;
	if (!value.isDigitStr()) return false;
	switch(data[1])
	{
	case '3' : 
		 // 130-139
		 return true;

	case '4' :
		 // 145, 147 
		 if (data[2] == '5' || data[2] == '7') return true;
		 break;

	case '5' :
		 // 150-153, 155-159
		 if (data[2] != '4') return true;
		 break;

	case '7' :
		 // 1700 1705 1709
		 if (data[2] == '0')
		 {
		 	if (data[3] == '0' || data[3] == '5' || data[3] == '9')
				return true;
		 }
		 // 176 177 178
		 else if (data[2] == '6' || data[2] == '7' || data[2] == '8')
		 {
			 return true;
		 }

	case '8' :
		 // 180-189
		 return true;

	default :
		 return false;
	}
	
	return false;
}


bool AosIsRaidFile(const u64 &fileId)
{
	return ((fileId & 0x8000000000000000LL) == 0x8000000000000000LL);
}


bool AosIsSuperIIL(const OmnString &iilname)
{
	if (iilname.length() <= 5) return false;
	return strncmp(iilname.data(), AOSZTG_SUPERIIL, 5) == 0;
}


bool AosIILRebuildBitmap(const OmnString &iilname, const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->rebuildBitmap(iilname, rdata);
}

