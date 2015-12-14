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
#include "DocServer/ProcIsDocDeleted.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcIsDocDeleted::AosProcIsDocDeleted(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_ISDOCDELETED, AosDocSvrProcId::eIsDocDeleted, regflag)
{
}


AosProcIsDocDeleted::~AosProcIsDocDeleted()
{
}


bool
AosProcIsDocDeleted::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
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

	//bool result = AosXmlDoc::isDocDeleted(docid);
	bool result = false;
	bool rslt = AosDocSvrSelf->isDocDeleted(docid, result, rdata);	
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	
	OmnString contents = "<Contents><record ";
	contents << "result" << "=\"" << (result?"true":"false")<< "\" "
			    << " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcIsDocDeleted::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcIsDocDeleted(false);
	return proc;
}
#endif
