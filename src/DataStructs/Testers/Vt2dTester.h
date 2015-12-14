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
#ifndef Aos_DataStructs_Testers_Vt2dTester_h
#define Aos_DataStructs_Testers_Vt2dTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include "DataStructs/Vector2D.h"
#include "DataStructs/Testers/Ptrs.h"


class AosVt2dTester : virtual public OmnTestPkg
{
private:
	AosRundataPtr 			mRundata;
	AosXmlTagPtr			mVtConf;
	AosXmlTagPtr			mVtConfDoc;
	AosDataGenPtr			mDataGen;
	map<OmnString, AosStrValueInfo> mFieldInfo;
	OmnString				mValueName;
public:
	AosVt2dTester();
	~AosVt2dTester();

	virtual bool start();

private:
	bool		basicTest();

	bool		config();

	bool		run();

	bool checkValue();
};
#endif
