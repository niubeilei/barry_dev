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
// 04/23/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataScanner_MultiFileScanner_h
#define AOS_DataScanner_MultiFileScanner_h

#include "DataScanner/DataScanner.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/ReadFile.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/ActionCaller.h"

class AosMultiFileScanner;

struct MultiFileScannerReq
{
	u64 reqId;
	int64_t blockSize;
	AosNetFileObjPtr file;
	AosRundataPtr rdata;
};


class AosMultiFileScanner : virtual public AosDataScanner, public OmnThreadedObj, public AosFileReadListener  
{
	OmnDefineRCObject;

	enum
	{
		eMaxMemory = 1000000000 // 1G
	};

private:

	static OmnThreadPtr							smThread;
	static OmnMutexPtr     						smLock;
	static OmnCondVarPtr   						smCondVar;
	static queue<MultiFileScannerReq>			smQueue;

	OmnMutexPtr     							mMemLock;
	OmnCondVarPtr   							mMemCondVar;
	vector<AosNetFileObjPtr>					mFiles;
	u64											mTotalReceivedSize;
	AosActionCallerPtr							mActions;
	AosRundataPtr								mRundata;

public:
	AosMultiFileScanner(const bool flag);
	AosMultiFileScanner(const AosRundataPtr &rdata);
	~AosMultiFileScanner();

	virtual AosDataScannerObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// AosFileReadListener Interface
	virtual void fileReadCallBack(
			const u64 &reqId, 
			const int64_t &expected_size, 
			const bool &finished,
			const AosDiskStat &disk_stat);
	virtual AosJimoPtr cloneJimo()  const;
	virtual int getPhysicalId() const;

	virtual void  addRequest(const u64 &reqId, const int64_t &blocksize);

	void setActionsCaller(const AosActionCallerPtr &caller) {mActions = caller;}

	void setFiles(const vector <AosNetFileObjPtr> &vv);

	void destroyedMember();

	AosBuffPtr 	getNextBlock(const u64 &reqId, const int64_t &expected_size); 

private:
	void	startThread();

	bool 	readFile(const MultiFileScannerReq &req);

};
#endif

