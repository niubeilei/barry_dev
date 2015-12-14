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
// 10/22/2010	Created by Chen Ding
// 2015/06/07   Moved from SEUtilServer
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_CloudidSvr_h
#define AOS_SEUtil_CloudidSvr_h

#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Docid.h"
#include "Util/RCObject.h"
#include "XmlUtil/XmlTag.h"

class AosCloudidSvr
{
private:

	AosCloudidSvr();
	~AosCloudidSvr();

public:
	static OmnString
	getCloudid(const OmnString &docid, const AosRundataPtr &rdata)
	{
		if (docid == "") return "";
		u64 dd = AosDocid::convertToU64(docid);
		return getCloudid(dd, rdata);
	}

	static OmnString
	getCloudid(const u64 &docid, const AosRundataPtr &rdata);
};

#endif
