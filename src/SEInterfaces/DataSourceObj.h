////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataSourceObj_h
#define Aos_SEInterfaces_DataSourceObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "ThreadShellRunners/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosValueRslt;

class AosDataSourceObj1 : virtual public OmnRCObject
{
private:
	// Commented out by Chen Ding, 2013/11/21
	// static AosDataSourceObjPtr		smDataSourceObj;

public:
	// virtual vector<OmnString> & getDistrMap() = 0;
	// virtual bool procFinished(const AosDistrBlobToBucketsRunnerPtr &runner) = 0;
	// virtual OmnFilePtr getFile(const int file_idx, const AosRundataPtr &rdata) const = 0;
	virtual void resetMemberLoop() = 0;
	virtual bool hasMore() const = 0;
	virtual AosDataScannerObjPtr nextScanner(const AosRundataPtr &rdata) = 0;
	virtual AosDataSourceObjPtr cloneDataSource(const AosRundataPtr &rdata) = 0;
	virtual int64_t getTotalSize() const = 0;
	virtual bool addDataScanner(
					const AosRundataPtr &rdata, 
					const AosDataScannerObjPtr &file) = 0;

	static AosDataSourceObjPtr createDataSourceStatic(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &worker_doc, 
					const AosXmlTagPtr &jimo_doc);

	// Commented out by Chen Ding, 2013/11/21
	// static AosDataSourceObjPtr getDataSourceObj() {return smDataSourceObj;}
	// static void setDataSourceObj(const AosDataSourceObjPtr &d) {smDataSourceObj = d;}
};
#endif
