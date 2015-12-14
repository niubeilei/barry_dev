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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetMulti_h
#define Aos_Dataset_DatasetMulti_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/RecordsetObj.h"
#include "Dataset/Dataset.h"


class AosDatasetMulti : public AosDataset
{
	OmnDefineRCObject;

private:
	vector<AosDatasetObjPtr>  	mDatasetList;
	bool					  	mIsUnionAll;
	set<OmnString>				mUnionSet;

	//number of records to get each time
	//mPageSize = 0 means no limitation
	u64							mPageSize;
	u64							mNumValues;
	u64							mTotalValues;

public:
	AosDatasetMulti();
	~AosDatasetMulti();
	
	virtual bool nextRecordset(
					const AosRundataPtr &rdata, 
					AosRecordsetObjPtr &recordset);
	
	virtual bool getRsltInfor(
					AosXmlTagPtr &infor,
					const AosRundataPtr &rdata);

	//Phil 2015/09/03
	virtual bool dataUnion(
			const AosRundataPtr &rdata,
			AosDatasetObjPtr &ds);

	virtual bool dataUnionAll(
			const AosRundataPtr &rdata,
			AosDatasetObjPtr &ds);

	void	setPageSize(u64 num) { mPageSize = num; }

};
#endif



