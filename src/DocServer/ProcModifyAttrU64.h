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
// 07/29/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////

#if 0
#ifndef Aos_DocServer_ProcModifyAttrU64_h
#define Aos_DocServer_ProcModifyAttrU64_h


#include "DocServer/DocSvrProc.h"
#include "DocServer/Ptrs.h"


class AosProcModifyAttrU64 : virtual public AosDocSvrProc
{
private:

public:
	AosProcModifyAttrU64(const bool regflag);
	~AosProcModifyAttrU64();

	virtual AosDocSvrProcPtr clone();
	virtual bool proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request);
};
#endif
#endif
