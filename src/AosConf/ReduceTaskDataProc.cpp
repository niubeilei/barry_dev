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

#include "AosConf/ReduceTaskDataProc.h"
#include "AosConf/DataRecordCtnr.h"
#include "XmlUtil/SeXmlParser.h"

#include <boost/make_shared.hpp>

using AosConf::ReduceTaskDataProc;
using boost::make_shared;


ReduceTaskDataProc::ReduceTaskDataProc()
{
	mDataRecordConf = "";
}

ReduceTaskDataProc::ReduceTaskDataProc(const OmnString &type)
{
	mDataRecordConf = "";
	mAttrs["type"] = type;
}


ReduceTaskDataProc::~ReduceTaskDataProc()
{
}


void 	
ReduceTaskDataProc::setCollectorType(const string &type)
{
	mCollectorAttrs["zky_type"] = type;
}


void 	
ReduceTaskDataProc::setAssemblerType(const string &type)
{
	mAssemblerAttrs["type"] = type;
}


void 	
ReduceTaskDataProc::setRecordType(const string &type)
{
	mRecordAttrs["type"] = type;
}


void	
ReduceTaskDataProc::setCollectorAttr(
		const string &name, 
		const string &value)
{
	mCollectorAttrs[name] = value;
}


void 	
ReduceTaskDataProc::setAssemblerAttr(
		const string &name,
		const string &value)
{
	mAssemblerAttrs[name] = value;
}

void 	
ReduceTaskDataProc::setRecord(const boost::shared_ptr<DataRecord> record)
{
	mDataRecord = record;
	mDataRecordConf = record->getConfig();
}


void    
ReduceTaskDataProc::setRecord(const string &dataRecordConf)
{
	AosXmlTagPtr conf = AosXmlParser::parse(dataRecordConf AosMemoryCheckerArgs);
	aos_assert(conf);
	string recordType = conf->getAttrStr("type");

	string tmpConf = dataRecordConf;
	if (recordType != "ctnr") 
	{
		string recordName = conf->getAttrStr("zky_name");
		boost::shared_ptr<DataRecordCtnr> ctnrRecordObj = boost::make_shared<DataRecordCtnr>();
		ctnrRecordObj->setAttribute("zky_name", recordName);
		ctnrRecordObj->setRecord(dataRecordConf);
		tmpConf = ctnrRecordObj->getConfig();
	}

	mDataRecordConf = tmpConf;
}


void 	
ReduceTaskDataProc::setRecordAttr(
		const string &name,
		const string &value)
{
	mRecordAttrs[name] = value;
}

/*
void 	
ReduceTaskDataProc::setCompareFunAttr(
		const string &key,
		const string &value)
{
	mCompareFunAttrs[key] = value;
}

void 
ReduceTaskDataProc::setCmpFieldAttrs(
		const DataProcCreateDatasetDoc::AosCmpField &value)
{
	mCmpFieldAttrs.push_back(value);
}


void 	
ReduceTaskDataProc::setCompareFunAttr(
		const string &key,
		int value)
{
	char buff[100]; 
	sprintf(buff, "%d", value);
	mCompareFunAttrs[key] = buff;
}
*/

string
ReduceTaskDataProc::getConfig()
{
	//	dataproc format
	// 	<dataproc>
	// 		<datacollector ...>
	// 			<asm ...>
	// 				<datarecord>...</datarecord>
	// 			</asm>
	// 			<CompareFun .../>
	// 		</datacollector>
	// 	</dataproc>

	// 1. dataproc node
	string config = "<dataproc ";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	// 2. datacollector node
	config += "<datacollector ";
	for (map<string, string>::iterator itr=mCollectorAttrs.begin(); 
			itr!=mCollectorAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	// 3. asm node
	config += "<asm ";
	for (map<string, string>::iterator itr=mAssemblerAttrs.begin(); 
			itr!=mAssemblerAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	//if (mDataRecord) config += mDataRecord->getConfig();
	if (mDataRecordConf != "") config += mDataRecordConf;

	config += "</asm>";

	// 4. CompareFun node
	if(mCmpFun)
	{
		config += mCmpFun->getConfig();
	}

	config += "</datacollector>";
	config += "</dataproc>";
	return config;
}
