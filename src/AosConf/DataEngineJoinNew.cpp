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

#include "AosConf/DataEngineJoinNew.h"

using AosConf::DataEngineJoinNew;

DataEngineJoinNew::DataEngineJoinNew()
{
	mAttrs["zky_type"] = "dataengine_join";
}

void	
DataEngineJoinNew::setConditionAttr(
		const string &name, 
		const string &value)
{
	mConditionAttrs[name] = value;
}
	
void 	
DataEngineJoinNew::setDataProc(
		const string &name, 
		const boost::shared_ptr<DataProc> &proc)
{
	map<string, vector<boost::shared_ptr<DataProc> > >::iterator itr;
	itr = mDataProcs.find(name);
	if(itr == mDataProcs.end())
	{
		vector<boost::shared_ptr<DataProc> > vec;
		vec.push_back(proc);
		mDataProcs.insert(make_pair(name, vec));
	}
	else
	{
		(itr->second).push_back(proc);
	}
}

string	
DataEngineJoinNew::getConfig() 
{
	// 1. action node
	string conf = "<action";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	conf += ">";

	// 2. parameters node
	conf += "<parameters>";
	conf += "<var zky_name=\"first\" ref_name=\"" + mFirstRefName + "\"></var>";
	conf += "<var zky_name=\"next\" ref_name=\"" + mNextRefName + "\"></var>";
	conf += "<var zky_name=\"temp\">";
	conf += "<datarecord type=\"ctnr\" zky_name=\"temp\">";
	conf += "<datarecord type=\"fixbin\" zky_length=\"8\" zky_name=\"temp\">";
	conf += "<datafields>";
	conf += "<datafield type=\"bin_u64\" zky_length=\"8\" zky_name=\"docid\" zky_offset=\"0\"></datafield>";
	conf += "</datafields>";
	conf += "</datarecord>";
	conf += "</datarecord>";
	conf += "</var>";
	conf += "</parameters>";


	// 3. condition node
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

/*
	// 3. true_cond dataprocs node
	conf += "<zky_dataprocs name=\"cond\">";
	conf += "<dataproc type=\"join\">";
	conf += "<fields>";
	conf += "<zky_lhs zky_input_field_name=\"" + mLhsFieldName + "\" />";
	conf += "<zky_rhs zky_input_field_name=\"" + mRhsFieldName + "\" />";
	conf += "</fields>";
	conf += "</dataproc>";
	conf += "</zky_dataprocs>";
*/
	// 4. cond dataprocs node
	map<string, vector<boost::shared_ptr<DataProc> > >::iterator itr = mDataProcs.begin();
	for(; itr != mDataProcs.end(); ++itr)
	{
		conf = conf + "<zky_dataprocs" + " " + "name=\"" + itr->first +"\">";

		vector<boost::shared_ptr<DataProc> > vec = itr->second;
		for (size_t i=0; i<vec.size(); i++)
		{
			string proc_conf = vec[i]->getConfig();
			conf += proc_conf;
		}
		conf += "</zky_dataprocs>";
	}
	conf += "</action>";
	return conf;
}
