////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/DocReq.h"

#include "Debug/Debug.h"
#include "SearchEngine/DocServerCb.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

static int sgNumObjs = 0;

AosDocReq::AosDocReq(
		const ReqType type,
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &doc, 
		const u64 &uid,
		const AosDocSvrCbObjPtr &caller,
		const void *userdata, 
		const AosRundataPtr &rdata)
:
next(0),
root(root),
doc(doc),
caller(caller),
userdata((void *)userdata),
reqtype(type),
docid(0),
userid(uid),
synobj(false)
{
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	sgNumObjs++;
}


AosDocReq::~AosDocReq()
{
	sgNumObjs--;
}
