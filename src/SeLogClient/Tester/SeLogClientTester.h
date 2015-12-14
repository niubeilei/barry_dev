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
#ifndef Aos_SeLogClient_Tester_SeLogClientTester_h
#define Aos_SeLogClient_Tester_SeLogClientTester_h 

#include "SeLogClient/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"

#include <vector>

using namespace std;


class AosSeLogClientTester : public OmnTestPkg,
						virtual public OmnThreadedObj
{
private:
	OmnString 			mSsid;
	vector<OmnString>   mCobjids;
	vector<u64>			mLogids;

public:
	AosSeLogClientTester();
	~AosSeLogClientTester();

	virtual bool		start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };
	
private:
	bool 		basicTest(const AosRundataPtr &rdata);
	bool 		addVersion(const AosRundataPtr &rdata);
	bool		createContainer(const AosRundataPtr &rdata);
	u64			createLog(
				const OmnString &container,
				const OmnString &logname,
				const AosRundataPtr &rdata);
	bool		createLog(const AosRundataPtr &rdata);
	bool		queryLogs(const AosRundataPtr &rdata);
	OmnString	getSiteId();
	OmnString	getLogname();
	OmnString	getContainer();
	OmnString 	getQueryStr(const bool &reverse);
	OmnString 	getFieldName();
};
#endif
