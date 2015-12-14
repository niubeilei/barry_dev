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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////

#include "AosConf/DataEngineJoin.h"

using AosConf::DataEngineJoin;

DataEngineJoin::DataEngineJoin()
{
	mAttrs["zky_type"] = "dataengine_join";
}

void	
DataEngineJoin::setConditionAttr(
		const string &name, 
		const string &value)
{
	mConditionAttrs[name] = value;
}

string	
DataEngineJoin::getConfig() 
{
	// 1. action node
	string conf = "<action";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	conf += ">";

	// 2. condition node
	conf += "<condition";
	for(map<string, string>::iterator itr=mConditionAttrs.begin(); 
			itr != mConditionAttrs.end(); ++itr)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	conf += "<zky_lhs dataset_name=\"" + mLhsDatasetName + "\" />";
	conf += "<zky_rhs dataset_name=\"" + mRhsDatasetName + "\" />";
	conf += "</condition>";

	// 3. true_cond dataprocs node
	conf += "<zky_dataprocs name=\"cond\">";
	conf += "<dataproc type=\"join\">";
	conf += "<fields>";
	conf += "<zky_lhs zky_input_field_name=\"" + mLhsFieldName + "\" />";
	conf += "<zky_rhs zky_input_field_name=\"" + mRhsFieldName + "\" />";
	conf += "</fields>";
	conf += "</dataproc>";
	conf += "</zky_dataprocs>";

	// 4. false_cond dataprocs node
	conf = conf + "<zky_dataprocs" + " " + "name=\"" + mCondType +"\">";

	for (size_t i=0; i<mDataProcsConf.size(); i++)
	{
		conf += mDataProcsConf[i];
	}
	conf += "</zky_dataprocs>";
	conf += "</action>";
	return conf;
}


