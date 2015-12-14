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
// 07/06/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcDeleteBinaryDoc.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosProcDeleteBinaryDoc::AosProcDeleteBinaryDoc(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_DELETEBINARYDOC, AosDocSvrProcId::eDeleteBinaryDoc, regflag)
{
}


AosProcDeleteBinaryDoc::~AosProcDeleteBinaryDoc()
{
}


bool
AosProcDeleteBinaryDoc::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosXmlTransPtr &trans)
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)
	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	    OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}
	
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	bool rslt = AosDocSvr::getSelf()->deleteBinaryDoc(docid, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcDeleteBinaryDoc::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcDeleteBinaryDoc(false);
	return proc;
}

#endif
