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
// 11/10/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/Sdoc00002.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
//#include "DocProc/DocProc.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEUtil/IILName.h"

AosSdoc00002::AosSdoc00002(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_00002, AosSdocId::eSdoc00002, flag)
{
}


AosSdoc00002::~AosSdoc00002()
{
}


bool
AosSdoc00002::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This smart doc rebuilds an IIL.
	//<sdoc zky_sdoctp="00002" 
	//		zky_objid="sdoc_rebuildiil" 
	//		composetype="attr" 
	//		attrname="gic_type" 
	//		startdocid="5000" 
	//		maxdocid="345911"/>
	
	// Indexing type. It can be:
	// 	1. Global Attribute Indexing
	// 	2. Global Keyword Indexing
	// 	3. Global Attribute Keyword Indexing
	// 	4. Container Attribute Indexing
	// 	5. Container keyword Indexing
	// 	6. Container Attribute Keyword Indexing
	// 	7. Container Member Create Time
	// 	8. Container Member Objid
	aos_assert_rr(sdoc, rdata, false);
	OmnString type = sdoc->getAttrStr("composetype", "");
	aos_assert_rr(type != "", rdata, false);

	OmnString attrname = sdoc->getAttrStr("attrname", "");
	aos_assert_rr(attrname != "", rdata, false);

	OmnString iiltype = sdoc->getAttrStr("iiltype", "str");

	OmnString iilname = AosIILName::composeAttrIILName(attrname);
	aos_assert_rr(iilname != "", rdata, false);
	
	u64 startdocid = sdoc->getAttrU64("startdocid", 5000);
	u64 maxdocid = sdoc->getAttrU64("maxdocid", 0);
	aos_assert_rr(maxdocid != 0 && maxdocid>=startdocid, rdata, false);

	if (iiltype == "str")
	{
		OmnString attrvalue = "";
		for(u64 docid = startdocid; docid <= maxdocid; docid++)
		{
			AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
			if (!doc) continue;
			attrvalue = doc->getAttrStr(attrname, "");
			if (attrvalue == "") continue;
			bool rslt = AosIILClientObj::getIILClient()->addStrValueDoc(
					iilname, attrvalue, docid, false, true, rdata);
			if (!rslt)
			{
				OmnAlarm << "\naddStrValueDoc failed, " 
						 << "\niilname: " << iilname 
						 << "\nattrvalue: " << attrvalue
						 << "\ndocid: " << docid << enderr;
			}
		}
	}
	else if (iiltype == "u64")
	{
		u64 attrvalue = 0;
		for(u64 docid = startdocid; docid <= maxdocid; docid++)
		{
			AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
			if (!doc) continue;
			attrvalue = doc->getAttrU64(attrname, 0);
			if (attrvalue == 0) continue;
			bool rslt = AosIILClientObj::getIILClient()->addU64ValueDoc(
					iilname, attrvalue, docid, false, true, rdata);

			if (!rslt)
			{
				OmnAlarm << "\naddStrValueDoc failed, " 
						 << "\niilname: " << iilname 
						 << "\nattrvalue: " << attrvalue
						 << "\ndocid: " << docid << enderr;
			}
		}
	}
	return true;
}

