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
#ifndef AOS_JimoLogic_JLStatistic_h
#define AOS_JimoLogic_JLStatistic_h

#include "JimoProg/Ptrs.h"
#include "JimoLogic/JimoLogic.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "AosConf/JobTask.h"  
#include "AosConf/Job.h"
#include "AosConf/DataSet.h"
#include "AosConf/JobTask.h"
#include "SEUtil/JqlTypes.h"
#include <vector>

class AosJLStatistic: public AosJimoLogic
{
	OmnDefineRCObject;
	
	struct StatRunMeasure
	{
		OmnString		name;
		OmnString 	 	agr_type;
		OmnString	 	field_name;
		OmnString		grpby_time_unit;	// Ketty 2014/10/10	
	};

	struct StatRunInfo
	{
		u32						mStatLevel;
		vector<OmnString>		mKeyFields;
		vector<StatRunMeasure>	mMeasures;
	};

private:
	vector<StatRunInfo>		mSubStats;
	JQLTypes::OpType 							mOp;
	
public:
	AosJLStatistic(
			const boost::shared_ptr<AosConf::DataSet> &inputds, 
			const AosXmlTagPtr &def,
			map<OmnString, AosXmlTagPtr> &table_fields,
			JQLTypes::OpType op=JQLTypes::eCreate);

	AosJLStatistic(int version);
	~AosJLStatistic();

	virtual AosJimoPtr cloneJimo()const {return 0;}
	virtual void setOp(JQLTypes::OpType op) { mOp = op; }

private:
	bool	init(const AosXmlTagPtr &input_dataset_conf, 
				const AosXmlTagPtr &run_stat_doc,
				map<OmnString, AosXmlTagPtr> &table_fields);

};
#endif

