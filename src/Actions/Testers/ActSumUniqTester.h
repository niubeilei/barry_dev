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
#ifndef Aos_Actions_Testers_ActSumUniqTester_h
#define Aos_Actions_Testers_ActSumUniqTester_h

#include "Actions/Ptrs.h"
#include "Actions/ActSumUniq.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/Ptrs.h"
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
using namespace std;


class AosActSumUniqTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr			mRundata;
	//AosTaskObjPtr			mTask;
	vector<OmnString>		mStrs;
	vector<u64>				mInts;
	int						mNum;
	OmnString 				mObjid;
	map<OmnString, u64>		mKeys;
	map<u64, OmnString>		mValues;

public:
	AosActSumUniqTester();
	~AosActSumUniqTester();

	virtual bool start();

private:
	bool		basicTest();
	bool		runAction(AosActSumUniq &action, const OmnString &str);
	AosXmlTagPtr	config();
	bool		finish(AosActSumUniq &action);
	bool		verify(const AosXmlTagPtr &conf);
	bool		createData(AosActSumUniq &action);
	bool 		verifyMapDoc();

	bool 		verifyToInt(
					const AosXmlTagPtr &def,
					AosDataType::E &data_type);

	bool 		verifyToStr(
					const AosXmlTagPtr &def,
					const AosDataType::E &data_type);
	void 		clearData();
};
#endif
