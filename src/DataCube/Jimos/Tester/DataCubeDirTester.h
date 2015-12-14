////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
// Modification History:
// 09/18/2013 Create by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCube_Jimos_Tester_DataCubeDirTester_h
#define Aos_DataCube_Jimos_Tester_DataCubeDirTester_h 

#include "SeLogClient/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Opr.h"
#include "Tester/TestPkg.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "SEInterfaces/DataCubeObj.h"

#include <vector>
#include <map>

using namespace std;


class AosDataCubeDirTester : public OmnTestPkg,
								public AosDataCubeCallerObj
{
	OmnDefineRCObject;
private:
	OmnString 			mSsid;
	OmnString 			mCrtFileName;
	OmnFilePtr			mCrtFile;
	bool				mIsFinished;
	AosRundataPtr 		mRundata;
	AosDataCubeObjPtr	mDataCube;

public:
	AosDataCubeDirTester();
	~AosDataCubeDirTester();

	virtual bool start();

	bool 		readDirTest(const AosRundataPtr &rdata);

	bool		writeDataIntoFile(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data);
	void 		callBack(
					const u64 &reqid,
					const AosBuffDataPtr &buff_data,
					bool finished);
private:
	bool 		basicTest(const AosRundataPtr &rdata);
	bool 		createNewFile(
					const AosRundataPtr &rdata,
					const AosBuffDataPtr &buff_data);

};
#endif
