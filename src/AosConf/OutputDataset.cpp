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

#include "AosConf/OutputDataset.h"
using AosConf::OutputDataset;

OutputDataset::OutputDataset()
{
}


OutputDataset::~OutputDataset()
{
}


void
OutputDataset::setCollerctorAttr(const string &name, const string &value)
{
	if (name != "") mDataCollectorAttrs[name] = value;
}


void
OutputDataset::setAsmAttr(const string &name, const string &value)
{
	if (name != "") mAsmAttrs[name] = value;
}


void 
OutputDataset::setShuffleType(const string &type)
{
	mShuffleType = type;
}


void 
OutputDataset::setRecord(const boost::shared_ptr<DataRecord> &record)
{
	mOutputRecord = record;
}


void 
OutputDataset::setCompFun(const boost::shared_ptr<CompFun> &fun)
{
	mCompFun = fun;
}


string
OutputDataset::getConfig()
{
	string conf = "";	
	conf += "<dataset ";
	for (map<string, string>::iterator itr = mAttrs.begin();
			itr != mAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";

	// datacollector attributes
	conf += "<datacollector group_type=\"" + mShuffleType + "\"";
	for (map<string, string>::iterator itr = mDataCollectorAttrs.begin();
			itr != mDataCollectorAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";

	// asm attributes
	conf += "<asm ";
	for (map<string, string>::iterator itr = mAsmAttrs.begin();
			itr != mAsmAttrs.end(); itr++)
	{
		conf += " " + itr->first + "=\"" + itr->second + "\"";
	}
	conf += ">";
	
	// 1. datarecord config
	if (mOutputRecord) conf += mOutputRecord->getConfig();

	conf += "</asm>";

	// 2. compFun config
	if (mCompFun) conf += mCompFun->getConfig();

	conf += "</datacollector>";
	conf += "</dataset>";

	return conf;
}
