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
// Modification History:
// 2013/11/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_CompoundDataset_h
#define Aos_Dataset_CompoundDataset_h

#include "Dataset/Dataset.h"
#include <vector>
using namespace std;


class AosCompoundDataset : public AosDataset
{
	OmnDefineRCObject;

private:
	OmnMutexPtr				mLock;
	OmnMutex*				mLockRaw;
	vector<AosDatasetPtr>	mDatasets;
	OmnString				mScanOrder;

public:
	AosCompoundDataset(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &doc);
	~AosCompoundDataset();

	virtual bool reset(const AosRundataPtr &rdata);
	virtual int64_t getNumEntries(const AosRundataPtr &rdata);
	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					const AosRecordsetObjPtr &recordset);

private:
	bool init(const AosRundataPtr &rdata);
	bool createDataScanner(const AosRundataPtr &rdata);
};
#endif

