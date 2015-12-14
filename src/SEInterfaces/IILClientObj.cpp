////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/IILClientObj.h"

#include "Rundata/Rundata.h"



AosIILClientObjPtr AosIILClientObj::smIILClient;


/*
bool 
AosIILClientObj::getDocidStatic(
		const AosRundataPtr &rdata, 
		const OmnString &iilname, 
		const OmnString &key, 
		u64 &docid)
{
	bool is_unique;
	aos_assert_rr(smIILClient, rdata, false);
	bool rslt = smIILClient->getDocid(iilname, key, eAosOpr_eq, false, docid, is_unique, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (!is_unique)
	{
		AosSetErrorUser(rdata, "iilclientobj_docid_not_unique") << docid << enderr;
		return false;
	}
	return true;
}
*/

