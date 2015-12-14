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
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_DocThrd_h
#define Aos_SEClient_Testers_DocThrd_h

#include "SearchEngine/Ptrs.h"
#include "SEClient/Ptrs.h"
#include "SEClient/Testers/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"



class AosDocThrd : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftWeight = 100
	};

private:
	AosSearchEngTesterPtr	mTorturer;
	int					mTotal;
	int					mNumDocs;
	int					mWeight;
	OmnIpAddr			mRemoteAddr;
	int					mRemotePort;
	AosSEClientPtr		mConn;
	OmnString			mAttrsTname;
	OmnString			mDocwdTname;
	OmnThreadPtr		mThread;

public:
	AosDocThrd(
		const AosSearchEngTesterPtr &torturer, 
		const OmnIpAddr &raddr, 
		const int rport, 
		const int total,
		const int weight);
	~AosDocThrd();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state,
						const OmnThreadPtr &thread);
  	virtual bool	signal(const int threadLogicId);
   	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool 	stop();

private:
	bool 	verifyDocs();
	bool 	verifyOneDoc(const int idx);
	bool 	verifyWords(const OmnString &docid);
	bool 	verifyAttrs(const OmnString &docid);
	bool 	verifyAttrs(const AosXmlTagPtr &doc);
	bool 	verifyRawDoc(const OmnString &doc1, const OmnString &docid);
	bool 	verifyCtnrs(const AosXmlTagPtr &doc);
};
#endif
