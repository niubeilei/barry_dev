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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogSvr_LogReq_h
#define Aos_LogSvr_LogReq_h

#include "LogSvr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosLogReq : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Reqid
	{
		eInvalid,

		eCreateLog,

		eMax
	};

private:
	Reqid			mReqid;
	AosXmlTagPtr	mDoc;
	AosRundata		mRundata;

public:
	AosLogReq(const Reqid reqid, const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	~AosLogReq() {}

	AosXmlTagPtr getDoc() const {return mDoc;}
	AosRundataPtr	getRundata() {AosRundataPtr rdata(&mRundata, false); return rdata;}
	OmnString getCloudid() const {return mRundata.getCid();}
	Reqid getReqid() const {return mReqid;}
};
#endif

