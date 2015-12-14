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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocServer_BinaryDocTesters_BinaryDocTester_h
#define Aos_DocServer_BinaryDocTesters_BinaryDocTester_h 

#include "DocServer/BinaryDocTesters/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"

class AosBinaryDocTester : public OmnTestPkg 
{

public:
	enum
	{
		eThreadNum = 20
	};

	OmnString 			  mValue[eThreadNum];
	u64 				  mStartDocid;
	vector<u64>           mDocids[eThreadNum];
private:

	struct TestThrd : public OmnThrdShellProc
	{
		OmnDefineRCObject;
		
		AosBinaryDocTesterPtr mCaller;

		int				mIdx;
		AosRundataPtr 	mRundata;

		TestThrd(
				const AosRundataPtr &rdata, 
				const int idx, 
				const AosBinaryDocTesterPtr &caller)
		:
		OmnThrdShellProc("test"),
		mCaller(caller),
		mIdx(idx),
		mRundata(rdata)
		{
		}

		bool run()
		{
			mCaller->proc(mIdx, mRundata);
			return true;
		}

		bool procFinished() {return true;}
	};


private:

public:
	AosBinaryDocTester();
	~AosBinaryDocTester();

	virtual bool		start();
	
private:
	bool 	basicTest(const AosRundataPtr &rdata);

	bool	proc(const int idx, const AosRundataPtr &rdata);

	u64	createData(const int idx, const AosRundataPtr &rdata);

	AosXmlTagPtr	generateData(const int idx, const AosRundataPtr &rdata);

	bool	createBinaryDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);

	bool	checkDoc(const u64 &docid, const int idx, const AosRundataPtr &rdata);

	bool	checkDoc(const int idx, const AosRundataPtr &rdata);

	AosXmlTagPtr retrieveBinaryDoc(
			const u64 &docid, 
			AosBuffPtr &doc_buff,
			const AosRundataPtr &rdata);
};
#endif
