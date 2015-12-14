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
#ifndef AOS_SEInterfaces_JimoLogicObj_h
#define AOS_SEInterfaces_JimoLogicObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "AosConf/DataField.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProc.h"
#include "AosConf/JobTask.h"
#include "AosConf/ReduceTaskDataProc.h"

using namespace AosConf;

class AosJimoLogicObj : public AosJimo
{
protected:
	static AosJimoLogicObjPtr smJimoLogicObj;

public:
	AosJimoLogicObj(const int version);
	~AosJimoLogicObj();


	virtual vector<boost::shared_ptr<DataField> > & getInputFields() = 0;
	virtual vector<boost::shared_ptr<DataField> > &getOutputFields() = 0; 
	virtual vector<boost::shared_ptr<DataSet> >& getOutputDataset() = 0;
	virtual vector<boost::shared_ptr<DataProc> > & getMapTaskDataProc() = 0;
	virtual vector<boost::shared_ptr<ReduceTaskDataProc> > &getReduceTaskDataProc() = 0;
	virtual vector<boost::shared_ptr<JobTask> > &getTasks() = 0;

	static AosJimoLogicObjPtr getJimoLogicStatic(AosRundata *rdata);

	static AosJimoLogicObjPtr createJimoLogicStatic(
						const AosXmlTagPtr &descriptor, 
						AosRundata *rdata);

	// JimoLogic Interface
	virtual AosJimoLogicObjPtr createJimoLogic(
						const AosXmlTagPtr &descriptor, 
						AosRundata *rdata) = 0;

private:
	static bool createJimoDocStatic(AosRundata *rdata);

};
#endif

