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
#ifndef Aos_DataJoin_DataJoinEngine_h
#define Aos_DataJoin_DataJoinEngine_h 

#include "Conds/Condition.h"
#include "DataJoin/DataJoin.h"
#include "SEInterfaces/DataProcObj.h"


class AosDataJoinEngine : public AosDataJoin
{
	struct WorkingData
	{
		OmnString 	key;
		u32			idx;
		bool		has_more;
		bool		is_smallest;
	};

	u32							mNumScanners;	
	vector<AosIILScannerObjPtr>	mIILScanners;
	list<WorkingData>			mWorkingData;
	AosDataAssemblerObjPtr		mIILAssembler;
	AosDataAssemblerObjPtr		mDocAssembler;
	AosDataRecordObjPtr			mIILRecord;
	AosDataRecordObjPtr			mDocRecord;
	AosGroupbyProcPtr			mGroupbyProc;
	vector<AosDataProcObjPtr>	mDataProcs;
	vector<AosConditionObjPtr>	mFilters;
	
public:
	AosDataJoinEngine();
	AosDataJoinEngine(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata);
	~AosDataJoinEngine();

	// OmnThreadedShellProc Interface
	virtual	bool	run();
	
	virtual int		getProgress();
	virtual bool	setQueryContext(
						const AosQueryContextObjPtr &context,
						const AosRundataPtr &rdata);

	static bool		compareWorkData(
						AosDataJoinEngine::WorkingData A,
						AosDataJoinEngine::WorkingData B);
	static int		compareKey(
						AosDataJoinEngine::WorkingData A,
						AosDataJoinEngine::WorkingData B);
	
private:
	virtual bool	config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	startJoin(const AosRundataPtr &rdata);
	virtual bool	runJoin(const AosRundataPtr &rdata);
	virtual AosDataJoinPtr	create(
						const AosXmlTagPtr &def,
						const AosDataJoinCtlrPtr &ctlr,
						const AosRundataPtr &rdata);
	virtual bool	checkConfig(
						const AosXmlTagPtr &def,
						const AosTaskObjPtr &task,
						const AosRundataPtr &rdata);
	
	void			sortWorkData();
	void			removeWorkData(const u32 idx);
	void			addWorkData(
						const OmnString &key,
						const u32 idx,
						const bool has_more);
	void			setWorkData(
						const OmnString &key,
						const u32 idx,
						const bool has_more);
	bool			getNextKey(
						const u32 idx,
						const AosRundataPtr &rdata);
	bool			createRecord(const AosRundataPtr &rdata);
	bool			createRecordRec(
						list<WorkingData>::iterator &itr,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	bool			procRecord(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	bool			addRecord(
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	bool			procGroupbyFinish(const AosRundataPtr &rdata);

	u64				getNextDocid(const AosRundataPtr &rdata);
	AosDataProcStatus::E processRecord(
						const u64 &docid,
						const AosRundataPtr &rdata);
};
#endif

