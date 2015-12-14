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
// 10/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessTimedPublic.h"

#include "MultiLang/LangDictMgr.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessTimedPublic::AosAccessTimedPublic()
:
AosAccess(AOSACCESSTYPE_TIMED_PUBLIC),
mDftPubDuration(eDftPubDuration)
{
}


bool 
AosAccessTimedPublic::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted, 
		bool &denied)
{
	// The tag should be:
	// 	<access start_attrname="xxx" 
	// 		len_attrname="xxx"
	// 		dft_length="xxx"/>
	
	granted = false;
	denied = false;
	aos_assert_rr(tag, rdata, false);

	AosXmlTagPtr accessedObj = sec_req.getAccessedObj();
	if (!accessedObj)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}

	OmnString start_attrname = tag->getAttrStr("start_attrname", AOSTAG_TIME_ATTRNAME);
	if (start_attrname == "")
	{
		AosSetEntityError(rdata, "accesstimedpub_missing_start_aname", 
				"Access", "TimedPublic") << tag << enderr;
		return false;
	}

	int startsec = accessedObj->getAttrInt(start_attrname, -1);
	u32 crtsec = OmnGetSecond();
	if (startsec < 0)
	{
		// It has not started yet. 
		OmnString csec;
		csec << crtsec;
		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		aos_assert_r(docclient, false);
		bool rslt = docclient->modifyAttrStr1(rdata,
							accessedObj->getAttrU64(AOSTAG_DOCID, 0), 
							accessedObj->getAttrStr(AOSTAG_OBJID, ""),
							start_attrname, csec, "0", false, false, true);

		if (!rslt)
		{
			AosSetErrorU(rdata, "Failed modifying attribute")
				<< ":" << accessedObj->getAttrStr(AOSTAG_OBJID);
			return false;
		}
		granted = true;
		rdata->setOk();
		return true;
	}

	if ((u32)startsec > crtsec)
	{
		// This is incorrect. 
		AosSetErrorU(rdata, "internal_error") << ":" << startsec << ":" << crtsec << enderr;
		OmnString csec;
		csec << crtsec;
		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		aos_assert_r(docclient, false);
		bool rslt = docclient->modifyAttrStr1(rdata,
							accessedObj->getAttrU64(AOSTAG_DOCID, 0), 
							accessedObj->getAttrStr(AOSTAG_OBJID, ""),
							start_attrname, csec, "0", false, false, true);

		if (!rslt)
		{
			AosSetErrorU(rdata, "Failed modifying attribute")
				<< ":" << accessedObj->getAttrStr(AOSTAG_OBJID);
			return false;
		}

		startsec = crtsec;
	}

	OmnString length_attrname = tag->getAttrStr("len_attrname", "zky_len_attrname");
	if (length_attrname == "")
	{
		AosSetErrorU(rdata, "internal_error");
		return false;
	}

	// Retrieve the time length.
	int length = accessedObj->getAttrInt(length_attrname, -1);
	if (length <= 0) 
	{
		// Retrieve the default length.
		length = tag->getAttrInt("dft_length", -1);
		if (length < 0)
		{
			logDenied(rdata, sec_req, tag, AOS_DENYREASON_TIMED_PUBLIC);
			denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
			return true;
		}
	}

	if (crtsec - startsec > (u32)length)
	{
		denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		logDenied(rdata, sec_req, tag, AOS_DENYREASON_TIMED_PUBLIC);
		return true;
	}

	granted = true;
	rdata->setOk();
	return true;
}


AosJimoPtr
AosAccessTimedPublic::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessTimedPublic(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

