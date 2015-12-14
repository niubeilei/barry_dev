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
#ifndef Aos_DataScanner_Testers_DocScannerTester_h
#define Aos_DataScanner_Testers_DocScannerTester_h

#include "Actions/Ptrs.h"
#include "IILClient/Ptrs.h"
#include "IILUtil/IILMatchType.h"
#include "IILUtil/IILEntrySelType.h"
#include "IILUtil/IILValueType.h"
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
#include <vector>
using namespace std;


class AosDocScannerTester : virtual public OmnTestPkg
{
private:
	AosDataScannerObjPtr	mScanner;
	map<u64, u64>		mDocids;
	int 				mNum;
	bool				mCreateDoc;
public:
	AosDocScannerTester();
	~AosDocScannerTester();

	virtual bool start();

private:
	AosXmlTagPtr	config(const AosRundataPtr &rdata);

	bool 			createScannerObj(
						const AosRundataPtr &rdata, 
						const AosXmlTagPtr &scanner_conf);

	void			modifyConfig(const AosXmlTagPtr  &config);

	bool			basicTest();

	bool			test(const AosRundataPtr &rdata);

	bool			createDoc(const AosRundataPtr &rdata);

	bool			proc(const AosRundataPtr &rdata);

	bool			verifyDoc(const AosBuffPtr &buff, const AosRundataPtr &rdata);

	bool			tryDocid(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);

};
#endif
