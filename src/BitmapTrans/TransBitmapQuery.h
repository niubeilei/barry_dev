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
// Modification History:
// 2013/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapTrans_TransBitmapQuery_h
#define Aos_BitmapTrans_TransBitmapQuery_h

//#include "BitmapUtil/BitmapReq.h"
//#include "BitmapUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Thread/ThreadShell.h"
#include "Thread/ThrdShellProc.h"
#include "TransUtil/BitmapTrans.h"
#include "Query/Ptrs.h"
#include <map>
using namespace std;

class AosTransBitmapQuery : virtual public AosBitmapTrans
{
	OmnDefineRCObject;
public:
	enum
	{
		eTestTermID = 777777777777ULL,
	};
private:
	AosTransBitmapQueryAgentObjPtr		mAgent;
	AosBitmapObjPtr						mRslt;
	OmnMutexPtr            		 		mLock;
	OmnCondVarPtr          	 			mCondVar;
public:
	AosTransBitmapQuery(const bool regflag);
//	AosTransBitmapQuery(const AosTransBitmapQuery &rhs);
	AosTransBitmapQuery(
			const AosRundataPtr &rdata, 
			const int cube_id,
			const u64 term_id,
			const vector<OmnString> &iilnames,
			const vector<AosQueryRsltObjPtr> &node_list,
			const vector<AosBitmapObjPtr> &partial_bitmaps, 
			const vector<u32> &expected_sections);
	~AosTransBitmapQuery(); 

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

	virtual bool					respCallBack();
	virtual bool 	wait(){mCondVar->wait(mLock);return true;}

	AosBitmapObjPtr getRslt(){return mRslt;}
	// OmnThrdShellProc interface
//	virtual bool    run();
//	virtual bool    procFinished();

//	static void setShowLog(const bool b);
//	static void setShowBitmap(const bool b);

/*	AosBmpBlockIndexObjPtr	getNodeSafe(
					const AosRundataPtr &rdata, 
					const u64 &node_id);

	// functions for threads
	bool							getProcNodeId(u32 &node_id, const int id);
	bool							getProcSectionID(u32 &local_id);
	vector<AosQueryRsltObjPtr>&		getNodeList();
	bool							getBitmapSection2(
										const AosRundataPtr &rdata, 
										const u64 &node_id,
										const u32 &local_id,
										const AosBitmapObjPtr &bitmap);
	AosBitmapObjPtr					getPartialSection(
										const AosRundataPtr &rdata, 
										const u32 &index, 
										const u32 &local_id);
	bool							setResult(const AosBitmapObjPtr &bitmap, 
										const u32 &local_id);
private:
	bool queryFailed(const AosRundataPtr &rdata, 
				const char *fname, 
				const int line, 
				const OmnString &errmsg);
	bool queryFinished(const AosRundataPtr &rdata, 
				const char *fname, 
				const int line, 
				const AosBitmapObjPtr &bitmap);
	// bool addPartialBitmap(
	// 			const AosBitmapObjPtr &partial_bitmap, 
	// 			const AosBitmapObjPtr &bitmap, 
	// 			const map<u64, bool> &sections);
	bool retrieveSortingIILBitmaps(
				const AosQueryRsltObjPtr &node_list, 
				AosBitmapObjPtr &bitmap); 
	bool check(const AosRundataPtr &rdatqa);

public:
	// Chen Ding, 2013/10/21
	bool procSectionedBitmap(
				const AosRundataPtr &rdata, 
				const vector<AosBitmapObjPtr> &bitmap);
	bool runQuery(const AosRundataPtr &rdata);
*/
};
#endif

