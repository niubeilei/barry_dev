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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcRetrieveDocids.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"

AosProcRetrieveDocids::AosProcRetrieveDocids(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_RETRIEVEDOCIDS, AosDocSvrProcId::eRetrieveDocids, regflag)
{
}


AosProcRetrieveDocids::~AosProcRetrieveDocids()
{
}


bool
AosProcRetrieveDocids::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocServer::retrieveDocids(...)
	int numdocids = request->getAttrInt("numdocids", -1);
	aos_assert_r(numdocids > 0, false);

	u64 docid = 0;
	bool rslt = AosDocSvrSelf->retrieveDocids(rdata, numdocids, docid);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	OmnString contents = "<Contents><record ";
	contents << "crtdocid" << "=\"" << docid << "\" "
		<< "numdocids"<< "=\"" << numdocids << "\" "
		<< " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcRetrieveDocids::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcRetrieveDocids(false);
	return proc;
}

#endif
