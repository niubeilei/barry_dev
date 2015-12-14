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
//
// Modification History:
// 06/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ModifyAccessRecord.h"

#include "API/AosApi.h"
#include "XmlUtil/AccessRcd.h"
#include "SEInterfaces/DocClientObj.h"
#include "Security/SecurityMgr.h"
#include "SEServer/SeReqProc.h"
#include "SeReqProc/ReqidNames.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Thread/Mutex.h"


AosModifyAccessRecord::AosModifyAccessRecord(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_MODIFY_ACCRCD, 
		AosSeReqid::eModifyAccessRecord, rflag)
{
}


bool 
AosModifyAccessRecord::proc(const AosRundataPtr &rdata)
{
	// This function modify the access record. 
	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString args = root->getChildTextByAttr("name", "args");
	OmnString owndocid;
	AosParseArgs(args, "owndocid", owndocid);
	if (owndocid == "")
	{
		rdata->setError() << "Missing docid!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	u64 docid = atoll(owndocid.data());
	if (docid == 0)
	{
		AosSetError(rdata, "eMissingDocid");
		OmnAlarm << rdata->getErrmsg() << ". Missing docid" << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr objdef = root->getFirstChild("objdef");
	AosXmlTagPtr arcd1;
	if (!objdef || !(arcd1 = objdef->getFirstChild()))
	{
		rdata->setError() << "Missing objdef!";
		AOSLOG_LEAVE(rdata);
		return false;
	}
	u64 arcd_docid = arcd1->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	OmnString docstr;
	docstr = arcd1->toString();
	aos_assert_r(docstr != "", false);

	//Ice AdmDoc
	AosAccessRcdPtr arcd = AosGetAccessRcd(rdata, 0, docid, "", false);
	if (!AosDocClientObj::getDocClient()->isRepairing() 
			&& !arcd && arcd_docid != AOS_INVDID)
	{
		rdata->setError() <<"record does not exist!";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, false);	
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(docid, doc, rdata); 
	aos_assert_r(rslt, false);

	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}

