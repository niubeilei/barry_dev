////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCollector_DataCollectorNorm_h
#define AOS_DataCollector_DataCollectorNorm_h

#include "API/AosApi.h"
#include "DataCollector/DataCollector.h"
#include "DataCollector/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <list>

using namespace std;

class AosDataCollectorNorm : virtual public AosDataCollector 
{
private: 
	//OmnMutexPtr			mLock;
	OmnString				mDataType;

public:
	//AosDataCollectorNorm(
	//		const AosXmlTagPtr &config, 
	//		const AosRundataPtr &rdata);

	AosDataCollectorNorm(const u64& job_id);

	~AosDataCollectorNorm();
	
	virtual bool addOutput(
				const AosXmlTagPtr &output, 
				const AosRundataPtr &rdata);

	virtual bool finishDataCollector(const AosRundataPtr &rdata);

	//virtual bool dataCollectorFinished(const AosRundataPtr &rdata);

	virtual bool config(const AosJobObjPtr &job,
					const AosXmlTagPtr &conf, 
					const AosRundataPtr &rdata);

private:
	bool addOutput(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &output, 
				const int &level);

};
#endif
