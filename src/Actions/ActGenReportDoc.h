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
// 09/12/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActGenReportDoc_h
#define Aos_SdocAction_ActGenReportDoc_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <vector>


class AosActGenReportDoc : virtual public AosSdocAction,
								   public OmnThreadedObj
{
private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr 				mThread;
	AosRundataPtr				mRundata;
	map<OmnString, OmnString>   mAttrs;
	AosXmlTagPtr				mQueryReq;
public:
	AosActGenReportDoc(const bool flag);
	AosActGenReportDoc(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosActGenReportDoc();

	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;


	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;

private:
	bool runQuery(const AosRundataPtr &rdata);
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

};
#endif

