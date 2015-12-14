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
// 07/29/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcIncrementValue.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"

AosProcIncrementValue::AosProcIncrementValue(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_INCREMENTVALUE, AosDocSvrProcId::eIncrementValue, regflag)
{
}


AosProcIncrementValue::~AosProcIncrementValue()
{
}


bool
AosProcIncrementValue::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosXmlDoc::isDocDeleted(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	     OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	     rdata->setSiteid(siteid);
    }

	OmnString aname = request->getAttrStr("aname", "");
	aos_assert_r(aname != "", false);

	u64 initvalue = request->getAttrU64("initvalue", 0);

	u64 incValue = request->getAttrU64("incvalue", 0);
	aos_assert_r(incValue, false);

	u64 newvalue = 0;
	bool rslt = AosDocSvr::getSelf()->incrementValue(rdata, docid, aname, 
			initvalue, incValue, newvalue, trans->getTransId());	
	aos_assert_r(rslt, false);
	
	OmnString contents = "<Contents><record ";
	contents << "rslt" << "=\"" << (rslt?"true":"false")<< "\" "
			 << "value" << "=\"" << newvalue << "\" "
			 << " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcIncrementValue::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcIncrementValue(false);
	return proc;
}
#endif
