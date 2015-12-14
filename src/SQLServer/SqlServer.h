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
// 06/12/2011	Created by Tracy Huang
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SqlInterface_SqlServer_h
#define Omn_SqlInterface_SqlServer_h

#include "Rundata/Ptrs.h"
#include "SQLServer/Ptrs.h"
#include "SqlUtil/SqlReqid.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "TransServer/TransProc.h"
#include "TransUil/Ptrs.h"



class AosSqlServer : public AosTransProc
{
private:
	AosSqlReqid::E		mId;
	OmnString			mReqidName;

public:
	AosSqlServer();
	~AosSqlServer();

	// SqlServer Interface
	//bool proc(const AosTransPtr &trans, const AosRundataPtr &rdata);
	virtual bool proc(
		const AosTransPtr &trans,
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata);
	bool sendResponse(const AosTransPtr &trans, const AosRundataPtr &rdata);
	bool startDatabase();
};
#endif

