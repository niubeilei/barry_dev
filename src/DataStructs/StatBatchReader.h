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
// 10/19/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_StatBatchReader_h
#define AOS_DataStructs_StatBatchReader_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
//#include "StatUtil/StatQueryRecord.h"
#include "DataStructs/Ptrs.h"
#include "DataStructs/StatReaderRequest.h"

class AosStatBatchReader :  public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eNumThrds = 5
	};

public:
	class AosStatReaderThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosVector2DPtr				mVector2d;
		AosStatReaderRequestPtr		mRequest;
		AosRundataPtr				mRundata;
	public:
		AosStatReaderThrd(
				const AosStatReaderRequestPtr &req,
				const AosVector2DPtr &vv,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("StatReader"),
		mVector2d(vv),
		mRequest(req),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};

	AosStatBatchReader();

	~AosStatBatchReader();

	bool shuffle(
			const AosRundataPtr &rdata,
			const AosVector2DPtr &vector2d,
			vector<u64> &dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeArea> &orig_time_conds,
			//vector<u64> &time_slots,
			const AosStatTimeUnit::E grpby_time_unit,
			const AosVector2DGrpbyRsltPtr &grpby_rslt);

};
#endif


