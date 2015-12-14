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
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActDeleteIIL_h
#define Aos_SdocAction_ActDeleteIIL_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "XmlUtil/Ptrs.h"


class AosActDeleteIIL : virtual public AosSdocAction
{
	class delThrd : public OmnThrdShellProc
	{
		friend class AosActDeleteIIL;

		OmnDefineRCObject;

		OmnString		mIILName;
		bool			mTrueDelete;
		AosRundataPtr	mRundata;
		
	public:
		delThrd(
			const OmnString &iilname,
			const bool true_delete,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("ActDelThrd"),
		mIILName(iilname),
		mTrueDelete(true_delete),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool    run();          
		virtual bool    procFinished(); 
	};

public:
	AosActDeleteIIL(const bool flag);
	AosActDeleteIIL(
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
	~AosActDeleteIIL();

	virtual bool run(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);
	
	virtual bool run(
			const AosTaskObjPtr &task, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);

	virtual bool checkConfig(
			const AosXmlTagPtr &sdoc,
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata) const;
	
};
#endif

