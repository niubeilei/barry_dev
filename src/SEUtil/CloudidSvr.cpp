////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/01/2011 Created by Chen Ding
// 2015/06/07 Moved from SEUtilServer by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/CloudidSvr.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/UserAcctObj.h"


OmnString
AosCloudidSvr::getCloudid(const u64 &docid, const AosRundataPtr &rdata)
{
	if (!docid) return "";
	if (docid < AOSDOCID_MAX_SYS_DOCID)
	{
		switch (docid)
		{
		case AOSDOCID_APPROOT:
			 return AOSCLOUDID_APPROOT;

		case AOSDOCID_DELOBJROOT:
			 return AOSCLOUDID_DELOBJROOT;

		case AOSDOCID_IILMGRROOT:
			 return AOSCLOUDID_IILMGRROOT;

		case AOSDOCID_SYSROOT:
			 return AOSCLOUDID_SYSROOT;

		case AOSDOCID_ROOT:
			 return AOSCLOUDID_ROOT;

		case AOSDOCID_GUEST:
			 return AOSCLOUDID_GUEST;

		default:
			 rdata->setError() << "Unrecognized system docid: " << docid;
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 return "";
		}
	}

	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, "");
	AosUserAcctObjPtr user_acct = docclient->getUserAcct(docid, rdata);
	if (!user_acct) return "";
	return user_acct->getAttrStr(AOSTAG_CLOUDID);
}
