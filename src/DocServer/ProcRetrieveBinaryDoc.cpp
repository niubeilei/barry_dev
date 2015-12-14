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
#include "DocServer/ProcRetrieveBinaryDoc.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosProcRetrieveBinaryDoc::AosProcRetrieveBinaryDoc(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_RETRIEVEBINARYDOC, AosDocSvrProcId::eRetrieveBinaryDoc, regflag)
{
}


AosProcRetrieveBinaryDoc::~AosProcRetrieveBinaryDoc()
{
}


bool
AosProcRetrieveBinaryDoc::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosXmlTransPtr &trans)
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

	AosBuffPtr buff;
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(docid, buff, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	
	OmnString contents = "<Contents><record><doc><![BDATA[";
	contents << buff->dataLen() << ":";
	contents.append(buff->data(), buff->dataLen());
	contents << "]]></doc></record></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcRetrieveBinaryDoc::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcRetrieveBinaryDoc(false);
	return proc;
}

#endif
