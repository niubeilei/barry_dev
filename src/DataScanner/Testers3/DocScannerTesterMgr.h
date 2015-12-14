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
// 09/26/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataScanner_Testers_DocScannerTesterMgr_h
#define Aos_DataScanner_Testers_DocScannerTesterMgr_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
//#include "IILUtil/IILMatchType.h"
//#include "IILUtil/IILEntrySelType.h"
//#include "IILUtil/IILValueType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "TaskMgr/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/Opr.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"
#include "DataScanner/DataScanner.h"
#include "DataScanner/Testers3/Ptrs.h"
#include <vector>
using namespace std;


class AosDocScannerTesterMgr : virtual public OmnTestPkg
{
private:
	bool		mCreated;	
	int			mCreateDocType;
	u64			mScannerNum;
	vector<AosXmlTagPtr> mConfig;
	u64 		mNum;
	vector<AosDocScannerTesterPtr> mVector;
	OmnString	mObjid;
	bool		mIsRetrieve;
	bool		mCreateConf;
public:
	AosDocScannerTesterMgr(const AosXmlTagPtr &def);
	~AosDocScannerTesterMgr();

	virtual bool start();

private:
	bool			basicTest();

	bool			createData(const AosRundataPtr &rdata);

	bool			create(const AosRundataPtr &rdata);
	bool			create1(const AosRundataPtr &rdata);

	bool			createFixedLengthDoc(const AosRundataPtr &rdata);
	bool			createFixedLengthDoc1(const AosRundataPtr &rdata);

	bool			createNormal(const AosRundataPtr &rdata);
	void			config();
};
#endif
