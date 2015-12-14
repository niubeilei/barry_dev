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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelObjid.h"

#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0


AosValueSelObjid::AosValueSelObjid(const bool reg)
:
AosValueSel(AOSACTOPRID_OBJID, AosValueSelType::eObjid, reg)
{
}


AosValueSelObjid::AosValueSelObjid(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_OBJID, AosValueSelType::eObjid, false)
{
}


AosValueSelObjid::~AosValueSelObjid()
{
}


bool
AosValueSelObjid::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	// item should be this format:
	// <zky_valuedef type="docid" zky_value_type="docid">
	// <item>
	//		<cond>
	//		xxxx,xxxx,||xxxx-xxxx
	//		</cond>
	// 		</cond>
	// 		<cond>
	// 		</cond>
	//      ......
	// </item>
	// </zky_valuedef>
	/////////////////////////////////////////////////
	
	AosXmlTagPtr item = doc->getFirstChild(true);
	AosXmlTagPtr child = item->getFirstChild(true);
	vector<OmnString> objids;
	vector<OmnString> objid_strs;
	AosStrSplit split("", ",");
	OmnString tmp;

	while (child)
	{
		tmp = child->getNodeText();
		split.splitStr(tmp.data(), ",");
		objid_strs = split.entriesV();
		for (u32 i = 0; i < objid_strs.size(); i++)
		{
			objids.push_back(objid_strs[i]);
			continue;
		}
		child = item->getNextChild();
	}
	
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert_r(queryRslt, false);

	AosIILClientObjPtr clinet = AosIILClientObj::getIILClient();
	for (u32 i = 0; i < objids.size(); i++)
	{
		u64 docid = clinet->getDocidByObjid(objids[i], rdata);
		queryRslt->appendDocid(docid);
	}
	valueRslt.setQueryRslt(queryRslt);	
	return true;
}


OmnString 
AosValueSelObjid::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelObjid::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelObjid(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
