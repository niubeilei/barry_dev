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
// 06/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_VFTester_h
#define Aos_Actions_Testers_VFTester_h

#include "Actions/Ptrs.h"
#include "IILAssembler/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include <vector>
using namespace std;


class AosVFTester : virtual public OmnTestPkg
{
private:
	u64			mSegmentSize;
	u64			mLastSegmentSize;
	u64			mNumSegment;
	u64			mFileSize;
	OmnString	mFileDataRead;
	OmnStirng	mFileData;
	OmnString	mObjid;
	OmnString	mAppendFileData;
	
	Vector<OmnString> mSegmentData;
	Vector<OmnString> mSegmentDataRead;
	AosRundataPtr	  mRdata;
	AosSyncTokenPtr	  mAppendFile;

	
public:
	AosVFTester();
	~AosVFTester();
	virtual bool start();

private:
	bool		basicTest();
	bool		config();
	bool		createFiles();
	bool		saveFiles();
	bool		splitFiles();
	bool		readFiles();
	bool		compareResults();
	bool		testSegmentRead();
	bool		testXmlConfigRead();

};
#endif

