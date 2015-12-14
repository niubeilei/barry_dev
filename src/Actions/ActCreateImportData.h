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
// 2012/02/23	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActCreateImportData_h
#define Aos_SdocAction_ActCreateImportData_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"


class AosActCreateImportData : virtual public AosSdocAction,
							   public OmnThreadedObj
{
	OmnThreadPtr 				mThread;
	AosXmlTagPtr				mConfig;
	AosTaskDataObjPtr			mTaskData;
	AosRundataPtr				mRundata;	

public:
	AosActCreateImportData(const bool flag);
	~AosActCreateImportData();

	virtual bool	run(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);
	
	virtual bool 	run(
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
	
	// OmnThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	bool createData(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

