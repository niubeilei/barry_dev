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
#ifndef AOS_JPLoadData_JPLoadData_h
#define AOS_JPLoadData_JPLoadData_h

#include "JimoProg/Ptrs.h"
#include "JimoProg/JimoProg.h"
#include "SEInterfaces/JimoProgObj.h"
#include "Thread/Ptrs.h"
#include "AosConf/JobTask.h"
#include "AosConf/Job.h"
#include "AosConf/DataSet.h"
#include "JimoLogic/JLPattern001.h"
#include <vector>


class AosJPLoadData : public AosJimoProg
{
	boost::shared_ptr<AosConf::DataSet> mInputds;
	OmnString mTableName;
	JQLTypes::OpType 			mOp;

public:
	AosJPLoadData(
		AosRundata *rdata,
		const AosXmlTagPtr &inputds, 
		const AosXmlTagPtr &tabledoc,
		list<string> &fields,
		JQLTypes::OpType &op);

	~AosJPLoadData();

	AosXmlTagPtr createConfig();

	vector<boost::shared_ptr<AosConf::DataSet> > collectInputDataSets();
	vector<boost::shared_ptr<AosConf::DataSet> > collectOutputDataSets();
	vector<boost::shared_ptr<AosConf::JobTask> > collectTasks();
	boost::shared_ptr<AosConf::ReduceTask> collectReduceTask();
	boost::shared_ptr<AosConf::MapTask> collectMapTask();

private:
	// Ketty 2014/10/11
	AosXmlTagPtr createRunStatDoc(
				const AosXmlTagPtr &statistic_def,
				const AosXmlTagPtr &stat_doc);



	// Ketty 2014/10/11
	bool 	saveNewRunStatDoc(
				const AosXmlTagPtr &statistic_def,
				const OmnString &stat_name,
				AosXmlTagPtr &new_run_stat_doc);

	virtual bool addStatTask(AosRundata*, OmnString&, OmnString&)
	{
		OmnNotImplementedYet;
		return false;
	}
};

#endif
