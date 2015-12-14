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
// 05/44/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_Actions_Testers_ActGroupTester_h
#define Aos_Actions_Testers_ActGroupTester_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
#include "DataBlob/Ptrs.h"

using namespace std;

class AosActGroupTester : virtual public OmnTestPkg
{
private:
	AosDataBlobPtr 		mBlobData;
	int					mCountRows;
	AosRundataPtr		mRundata;
	OmnDataStorePtr 	mStore;
	AosXmlTagPtr 		mActionConf;
	vector<u64>			mDocids;
	AosStrU64Array		mData;
	vector<OmnString>	mKeys;
	AosXmlTagPtr	 	mTestersConf; 
	AosXmlTagPtr	 	mBlobconf; 
	AosXmlTagPtr	 	mConf; 
	AosXmlTagPtr	 	mApp_conf;
	AosXmlTagPtr	 	mGroupconf; 
	int					mNumTries;
	int					mGroupIdLen;
	int					mRecordLen;
	
public:
	AosActGroupTester();
	~AosActGroupTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		insertDataToMySQL(
					const OmnString &key, 
					const OmnString &record, 
					const u64 &value);
	bool		MySQLInit();
	bool		MySQLClearData();
	bool		MySQLGetResult();
	bool		MySQLExec(OmnString sql );
	bool		getMysqlResult();
	bool		runMySQLGroup();
	bool		createRandomRows();
	bool		compareResults();
	bool		runAction();
	bool		config();
	bool		createBlob();
	bool		createData();
	bool		createRandomLenth();
	bool		testBuffArray(); 
	bool		xmlTest();
	bool		randomTest();
	bool		clearData();
	OmnString	composeKey(OmnString &key);
	
};
#endif
#endif
