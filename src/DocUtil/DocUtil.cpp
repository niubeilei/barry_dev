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
//
// Modification History:
// 07/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocUtil/DocUtil.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/IILClientObj.h"



bool
AosDocUtil::isCloudidBound(
		const OmnString &siteid, 
		const OmnString &cid, 
		u64 &userid, 
		const AosRundataPtr &rdata)
{
	// It checks whether the cloudid 'cid' is a "Real Cloudid". 
	// A cloud id is "Real" if it was bound to a user account.
	aos_assert_r(cid != "", false);
	OmnString iilname = AOSZTG_CLOUDID;
	iilname << siteid;
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, cid, userid, rdata);
	return (rslt && userid != AOS_INVDID);
}


