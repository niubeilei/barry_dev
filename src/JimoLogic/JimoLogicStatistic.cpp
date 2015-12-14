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
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicStatistic.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JQLStatement/JqlStatement.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcIndex.h"
#include "AosConf/Stat/StatConf.h"

#include <boost/make_shared.hpp>
using AosConf::StatConf;
using boost::make_shared;
using boost::shared_ptr;


AosJLStatistic::AosJLStatistic(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &def,
		map<OmnString, AosXmlTagPtr> &table_fields,
		JQLTypes::OpType op)
:AosJimoLogic(1)
{
	mOp = op;
	OmnString inputds_str = inputds->getConfig();	
	AosXmlTagPtr inputds_doc = AosXmlParser::parse(inputds_str AosMemoryCheckerArgs);       
	aos_assert(inputds_doc);
	// Ketty 2014/10/13
	init(inputds_doc, def, table_fields);
	/*
	StatConf stat(inputds_doc, def);
	bool rslt = stat.createConfig();
	aos_assert(rslt);

	mInputFields = stat.getInputFields();
	mOutputds = stat.getOutputDataset();
	mMapDataProcs = stat.getMapTaskDataProc();
	mReduceDataProcs = stat.getReduceTaskDataProc();
	mTasks = stat.getTasks();
	*/
}


AosJLStatistic::AosJLStatistic(int version)
:AosJimoLogic(version)
{
}


AosJLStatistic::~AosJLStatistic()
{
}


bool
AosJLStatistic::init(
		const AosXmlTagPtr &input_dataset_conf, 
		const AosXmlTagPtr &run_stat_doc,
		map<OmnString, AosXmlTagPtr> &table_fields)
{
	// Ketty 2014/10/13
	aos_assert_r(input_dataset_conf, false);

	//bool rslt = analyzeRunStatDoc(run_stat_doc);
	//aos_assert_r(rslt, false);

	StatConf stat(table_fields);
	stat.setOp(mOp);
	bool rslt = stat.configNew(input_dataset_conf, run_stat_doc, table_fields);
	aos_assert_r(rslt, false);

	mInputFields = stat.getInputFields();
	mOutputds = stat.getOutputDataset();
	mMapDataProcs = stat.getMapTaskDataProc();
	mReduceDataProcs = stat.getReduceTaskDataProc();
	mTasks = stat.getTasks();
	
	return true;
}



