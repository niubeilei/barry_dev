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

#include "AosConf/DataProcCreateFileInfo.h"
using AosConf::DataProcCreateFileInfo;

DataProcCreateFileInfo::DataProcCreateFileInfo()
{
	mAttrs["type"] = "createfileinfo";
}


DataProcCreateFileInfo::~DataProcCreateFileInfo()
{
}


void 	
DataProcCreateFileInfo::setCollectorType(const string &type)
{
	mCollectorAttrs["zky_type"] = type;
}


void 	
DataProcCreateFileInfo::setAssemblerType(const string &type)
{
	mAssemblerAttrs["type"] = type;
}


void 	
DataProcCreateFileInfo::setRecordType(const string &type)
{
	mRecordAttrs["type"] = type;
}


void	
DataProcCreateFileInfo::setCollectorAttr(
		const string &name, 
		const string &value)
{
	mCollectorAttrs[name] = value;
}


void 	
DataProcCreateFileInfo::setAssemblerAttr(
		const string &name,
		const string &value)
{
	mAssemblerAttrs[name] = value;
}

void 	
DataProcCreateFileInfo::setRecord(const boost::shared_ptr<DataRecord> record)
{
	mDataRecord = record;
}

void 	
DataProcCreateFileInfo::setRecordAttr(
		const string &name,
		const string &value)
{
	mRecordAttrs[name] = value;
}


void 	
DataProcCreateFileInfo::setCompareFunAttr(
		const string &key,
		const string &value)
{
	mCompareFunAttrs[key] = value;
}


void 
DataProcCreateFileInfo::setCmpFieldAttrs(
		const DataProcCreateDatasetDoc::AosCmpField &value)
{
	mCmpFieldAttrs.push_back(value);
}


string
DataProcCreateFileInfo::getConfig()
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
	string mConf = "<dataproc ";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		mConf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	mConf += ">";

	// 2. datacollector node
	mConf += "<datacollector ";
	for (map<string, string>::iterator itr=mCollectorAttrs.begin(); 
			itr!=mCollectorAttrs.end(); itr++)
	{
		mConf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	mConf += ">";

	// 3. asm node
	mConf += "<asm ";
	for (map<string, string>::iterator itr=mAssemblerAttrs.begin(); 
			itr!=mAssemblerAttrs.end(); itr++)
	{
		mConf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	mConf += ">";

	if (mDataRecord)
		mConf += mDataRecord->getConfig();

	mConf += "</asm>";

	// 4. CompareFun node
	mConf += "<CompareFun";
	for (map<string, string>::iterator itr=mCompareFunAttrs.begin(); 
			itr!=mCompareFunAttrs.end(); itr++)
	{
		mConf += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	mConf += ">";

	mConf += "<cmp_fields>";
	for ( u32 i = 0; i < mCmpFieldAttrs.size(); i++)
	{
		mConf += "<field";
		mConf += " cmp_size=\"" + mCmpFieldAttrs[i].size + "\"";
		mConf += " cmp_datatype=\"" + mCmpFieldAttrs[i].data_type + "\"";
		mConf += " cmp_pos=\"" + mCmpFieldAttrs[i].pos + "\"";
		mConf += " ></field>";
	}
	mConf += "</cmp_fields>";
	mConf +="</CompareFun>";

	mConf += "</datacollector>";
	mConf += "</dataproc>";
	return mConf;
}
