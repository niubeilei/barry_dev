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
// 10/22/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/ObjidSvr.h"

#include "SEUtil/IILName.h"
#include "SEUtil/Objid.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


bool
AosObjidSvr::ensureObjidUnique(
		OmnString &objid, 
		const OmnString &the_prefix, 
		const OmnString &cid, 
		const u64 &docid,
		const bool checkFirst, 
		const AosRundataPtr &rdata)
{
	// This function checks whether 'objid' is unique. If not, 
	// it will append (nn) to it until the objid is unique. 
	// Note that it will try only mMaxObjidTries number
	// of times. If failed, it will return false.
	// Note that objid should already contain cid. 
	//
	// IMPORTANT: This function is used by AosDocServer. When this function
	// is called, AosDocServer should have been locked because ensuring
	// objid unique is an exclusive call.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	if (cid == "")
	{
		objid = the_prefix;
	}
	else
	{
		objid = AosObjid::compose(the_prefix, cid);
	}
	aos_assert_rr(objid != "", rdata, false);

	if (checkFirst && objid != "")
	{
		OmnString iilname = AosIILName::composeObjidListingName(siteid);
		//bool rslt = AosIILClientSelf->addStrValueDoc(iilname, objid, docid, true, true, rdata);
		bool rslt = AosIILClientObj::getIILClient()->addStrValueDoc(iilname, objid, docid, true, true, rdata);
		if (rslt)
		{
			rdata->setOk();
			return true;
		}
	}

	int tries = 0;
	int guard = 0;
	OmnString prefix = the_prefix;
	while (guard++ < eNotPossible)
	{
		while (tries++ < eMaxObjidTries)
		{
			if (cid == "")
			{
				objid = prefix;
				objid << "(" << tries << ")";
			}
			else
			{
				objid = AosObjid::compose(prefix, tries, cid);
			}

			OmnString iilname = AosIILName::composeObjidListingName(siteid);
			//bool rslt = AosIILClientSelf->addStrValueDoc(iilname, objid, docid, true, true, rdata);
			bool rslt = AosIILClientObj::getIILClient()->addStrValueDoc(iilname, objid, docid, true, true, rdata);
			if (rslt) 
			{
				rdata->setOk();
				return true;
			}
		}

		// Not able to make it unique. Will try a new way:
		// 	prefix + "(" + seconds ")" + cid
		u32 secs = OmnGetUsec() % eSecLength;
		prefix << "(" << secs << ")";
		tries = 0;
	}

	rdata->setError() << "Too many objids with the same root: " << objid;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


