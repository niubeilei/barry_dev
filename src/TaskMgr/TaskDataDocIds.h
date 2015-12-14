////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013-05-21	Create by Young 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskMgr_TaskDataDocIds_h
#define AOS_TaskMgr_TaskDataDocIds_h

#include "TaskMgr/TaskData.h"


class AosTaskDataDocIds : public AosTaskData
{
	vector<u64>		mDocids;
	int				mPhysicalId;
	OmnString		mRowDelimiter;
	AosDataScannerType::E		mScannerType;
	
public:
	AosTaskDataDocIds(const bool flag);
	AosTaskDataDocIds(
			const vector<u64> &docids,
			const int physical_id,
			const OmnString &row_delimiter);
	~AosTaskDataDocIds();

	virtual int			getPhysicalId() const {return mPhysicalId;}

	virtual bool serializeTo(
				AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);
	virtual bool serializeFrom(
				const AosXmlTagPtr &xml,
				const AosRundataPtr &rdata);

	virtual AosTaskDataObjPtr create() const;
	virtual AosTaskDataObjPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata) const;
	
	virtual AosDataCacherObjPtr createDataCacher(
				const AosDataRecordObjPtr &record,
				const AosRundataPtr &rdata);

	virtual AosDataScannerObjPtr createDataScanner(const AosRundataPtr &rdata);
							
	virtual bool isDataExist(const AosRundataPtr &rdata);

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

};
#endif

#endif
