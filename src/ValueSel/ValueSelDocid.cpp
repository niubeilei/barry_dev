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
#include "ValueSel/ValueSelDocid.h"

#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/Opr.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelDocid::AosValueSelDocid(const bool reg)
:
AosValueSel(AOSACTOPRID_DOCID, AosValueSelType::eDocid, reg)
{
}


AosValueSelDocid::AosValueSelDocid(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_DOCID, AosValueSelType::eDocid, false)
{
}


AosValueSelDocid::~AosValueSelDocid()
{
}


bool
AosValueSelDocid::run(
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
	vector<u64> docids;
	vector<OmnString> docid_strs;
	AosStrSplit split("", ",");
	OmnString tmp;

	while (child)
	{
		tmp = child->getNodeText();
		split.splitStr(tmp.data(), ",");
		docid_strs = split.entriesV();
		for (u32 i = 0; i < docid_strs.size(); i++)
		{
			split.splitStr(docid_strs[i].data(), "-");
			if (split.entriesV().size() <= 1)
			{
				docids.push_back(docid_strs[i].toU64());
				continue;
			}
			if (split.entriesV().size() > 0 || split.entriesV().size() <= 2)
			{
				u64 start = (*(split.entriesV().begin())).toU64();
				if (start <= (u64)AosGetNumCubes()) start = AosGetNumCubes();
				u64 end = (*(split.entriesV().begin() + 1)).toU64();
				while(start <= end)
				{
					docids.push_back(start);
					start++;
				}
			}
		}
		child = item->getNextChild();
	}
	
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	aos_assert_r(queryRslt, false);

	for (u32 i = 0; i < docids.size(); i++)
	{
		queryRslt->appendDocid(docids[i]);
	}
	valueRslt.setQueryRslt(queryRslt);	
	return true;
}


OmnString 
AosValueSelDocid::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelDocid::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelDocid(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}
#endif
