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
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ImportData_ImportDataThrd_h
#define Aos_ImportData_ImportDataThrd_h

#include "alarm_c/alarm.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosImportDataThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftBuffSize = 10000000,	// 10M
		eMinReadSize = 10000000,	// 10M
		eDftReadSize = 100000000	// 100M
	};

protected:
	u32						mStartTime;
	u32						mEndTime;
	bool					mFinished;
	bool					mSuccess;
	
	AosImportDataCtlrPtr	mCtlr;
	AosDataCacherObjPtr		mDataCacher;
	AosRundataPtr			mRundata;
	bool					mNeedCreateDocid;
	
public:
	AosImportDataThrd(
			const AosDataCacherObjPtr &cacher,
			const AosImportDataCtlrPtr &controller,
			const AosRundataPtr &rdata);
	virtual ~AosImportDataThrd();
	
	bool 			isFinished() const {return mFinished;}
	bool 			isSuccess() const {return mSuccess;}
	
	OmnString		getStatusRecord() const;
	
	static bool		StaticInit(const AosXmlTagPtr &conf);
};
#endif

