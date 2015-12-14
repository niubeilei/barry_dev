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
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataJoin_DataJoinNorm_h
#define Aos_DataJoin_DataJoinNorm_h 

#include "Conds/Condition.h"
#include "DataJoin/DataJoin.h"
#include "SEInterfaces/DataProcObj.h"


class AosDataJoinNorm : public AosDataJoin
{
	struct WorkingData
	{
		const char *key;
		int			len;
		int			idx;
		bool		is_smallest;
	};

	int							mPrefixLen;
	int							mNumScanners;
	vector<AosIILScannerObjPtr>	mIILScanners;
	vector<WorkingData>			mWorkingData;
	vector<WorkingData>			mNewData;
	AosDataAssemblerObjPtr		mIILAssembler;
	AosDataAssemblerObjPtr		mDocAssembler;
	AosDataRecordObjPtr			mIILRecord;
	AosDataRecordObjPtr     	mDocRecord;
	vector<AosDataProcObjPtr>  	mDataProcs;
	AosGroupbyProcPtr			mGroupbyProc;
	vector<AosConditionObjPtr>	mFilters;

public:
	AosDataJoinNorm();
	AosDataJoinNorm(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinNorm();

	// OmnThreadedShellProc Interface
	virtual	bool	run();
	
	virtual int		getProgress();
	virtual bool	setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	bool			startJoin(const AosRundataPtr &rdata);
	bool			runJoin(const AosRundataPtr &rdata);
	bool			addRecord(const AosRundataPtr &rdata);
	bool			mergeData(const int num_values);
	bool			setSmallestFlag(const u32 nn);
	void			printWorkingData(const int num_new_data);
	bool			addToWorkingData(
						vector<AosDataJoinNorm::WorkingData> &working_data, 
						int &num_values,
						const char *data, 
						const int len,
						const u32 idx);
	bool			retrieveData(
						const int idx,
						const AosRundataPtr &rdata);
	bool 			allDataReceived() const;
	bool			procGroupbyFinish(const AosRundataPtr &rdata);
	
	u64				getNextDocid(const AosRundataPtr &rdata);
	AosDataProcStatus::E processRecord(
						const u64 &docid,
						const AosRundataPtr &rdata);
};
#endif

