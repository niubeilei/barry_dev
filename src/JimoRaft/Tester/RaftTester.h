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
// 2015-1-8 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoRaft_Tester_RaftTester_h
#define Aos_JimoRaft_Tester_RaftTester_h

#include "BlobSE/BlobSE.h"
#include "BlobSE/BlobSEAPI.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "UtilData/DfmDocType.h"
#include "DfmUtil/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Rundata/Rundata.h"
#include <ext/hash_map>
#include <map>
#include <vector>

class AosRaftTester : public OmnTestPkg
{
private:

public:
	AosRaftTester();
	~AosRaftTester();

	virtual bool		start();

private:
	//test cases
	bool	test1();
	bool	test2();
	bool	test3();
	bool	test4();
	bool	test5();
	bool	test6();
	bool	test7();
	bool	test8();
	bool	test9();
	bool	test10();
	bool	test11();
	bool	test12();

	//verification methods
	bool testFinished(
			OmnString testName, 
			bool rslt);
	bool checkAll(vector<u32> &nodeList);
	bool checkRole(vector<OmnString> &infoList);
	bool checkTermId(vector<OmnString> &infoList);
	bool checkLastLogId(vector<OmnString> &infoList);
	bool checkLogFile(vector<u32> &nodeList);

	//node control methods
	int     startNode(int nodeId);
	bool    stopNode(int nodeId);
	bool    stopAllNodes();
	
	//helper methods
	OmnString getLastLine(
			OmnString fName, 
			OmnString pattern);

	OmnString getKeyValue(
			OmnString line,
			OmnString keyBefore,
			OmnString keyAfter);

	OmnString getTermId(OmnString line);
	OmnString getLastLogId(OmnString line);
	OmnString getRole(OmnString line);

	bool getLogFileData(
			u32 nodeId,
			AosBuffPtr &buff);

};
#endif

