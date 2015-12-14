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
// 2014/8/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatServer_Vt2dReadReq_h
#define AOS_StatServer_Vt2dReadReq_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
//#include "DataStructs/Ptrs.h"
#include "StatUtil/StatTimeArea.h"
#include "Vector2D/Ptrs.h"
#include "Vector2DQryRslt/Ptrs.h"

class AosVt2dReadReq :  public OmnThrdShellProc 
{
	OmnDefineRCObject;

private:
	AosRundataPtr				mRdata;
	OmnSemPtr 					mSem;
	AosVector2DPtr				mVector2d;
	
	vector<AosStatTimeArea> 	mQryTimeAreas;
	vector<u64>     			mStatDocids;
	AosVt2dQryRsltProcPtr 		mQryRsltProc;
	AosVt2dQryRsltPtr			mQryRslt;

public:
	AosVt2dReadReq(
				const AosRundataPtr &rdata,
				const OmnSemPtr &sem,
				const AosVector2DPtr &vt2d,
				vector<AosStatTimeArea> &qry_time_areas,
				vector<u64> &stat_docids,
				const AosVt2dQryRsltProcPtr &qryrslt_proc,
				const AosVt2dQryRsltPtr	&qry_rslt);
	~AosVt2dReadReq();
		
	virtual bool run();
	virtual bool procFinished();
	
	//AosVt2dQryRsltPtr getQryRslt(){ return mQryRslt; };

};
#endif


