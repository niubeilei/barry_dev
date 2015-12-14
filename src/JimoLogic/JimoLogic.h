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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogic_JimoLogic_h
#define AOS_JimoLogic_JimoLogic_h

#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "AosConf/JobTask.h"  
#include "AosConf/Job.h"
#include "AosConf/DataSet.h"
#include "AosConf/ReduceTaskDataProc.h"
#include <vector>

using namespace AosConf;

class AosJimoLogic: public AosJimoLogicObj
{
	OmnDefineRCObject;

	vector<boost::shared_ptr<DataField> > 		mInputFields;
	vector<boost::shared_ptr<DataField> > 		mOutputFields;
	vector<boost::shared_ptr<DataSet> > 		mOutputds;
	vector<boost::shared_ptr<DataProc> > 		mMapDataProcs;
	vector<boost::shared_ptr<ReduceTaskDataProc> > mReduceDataProcs;
	vector<boost::shared_ptr<JobTask> > 		mTasks;

public:
	AosJimoLogic();
	AosJimoLogic(int version);
	~AosJimoLogic();

	virtual AosJimoLogicObjPtr createJimoLogic(
			                    const AosXmlTagPtr &descriptor,
								AosRundata *rdata);

	virtual vector<boost::shared_ptr<DataField> > &getInputFields()
	{
		return mInputFields;
	}
	virtual vector<boost::shared_ptr<DataField> > &getOutputFields()
	{
		return mOutputFields;
	}
	virtual vector<boost::shared_ptr<DataSet> > &getOutputDataset()
	{
		return mOutputds;
	}
	virtual vector<boost::shared_ptr<DataProc> > & getMapTaskDataProc()
	{
		return mMapDataProcs;
	}
	virtual vector<boost::shared_ptr<ReduceTaskDataProc> > &getReduceTaskDataProc()
	{
		return mReduceDataProcs;
	}
	virtual vector<boost::shared_ptr<JobTask> > & getTasks()
	{
		return mTasks;
	}

	bool setOutputDataset(const AosXmlTagPtr &def);
	bool setMapTaskDataProc(
				const boost::shared_ptr<DataSet> &inputds,
				const AosXmlTagPtr &def,
				map<OmnString, AosXmlTagPtr> &table_fileds);

	bool setReduceDataProc(
				const boost::shared_ptr<DataSet> &inputds,
				const AosXmlTagPtr &def);
};
#endif

