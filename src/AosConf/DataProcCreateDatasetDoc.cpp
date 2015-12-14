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

#include "AosConf/DataProcCreateDatasetDoc.h"
using AosConf::DataProcCreateDatasetDoc;

DataProcCreateDatasetDoc::DataProcCreateDatasetDoc()
{
	mAttrs["type"] = "createdatasetdoc";
}


DataProcCreateDatasetDoc::~DataProcCreateDatasetDoc()
{
}


void 	
DataProcCreateDatasetDoc::setCollectorType(const string &type)
{
	mCollectorAttrs["zky_type"] = type;
}


void 	
DataProcCreateDatasetDoc::setAssemblerType(const string &type)
{
	mAssemblerAttrs["type"] = type;
}


void 	
DataProcCreateDatasetDoc::setRecordType(const string &type)
{
	mRecordAttrs["type"] = type;
}


void	
DataProcCreateDatasetDoc::setCollectorAttr(
		const string &name, 
		const string &value)
{
	mCollectorAttrs[name] = value;
}


void 
DataProcCreateDatasetDoc::setCmpFieldAttrs(
		const DataProcCreateDatasetDoc::AosCmpField &value)
{
	mCmpFieldAttrs.push_back(value);
}


void 	
DataProcCreateDatasetDoc::setAssemblerAttr(
		const string &name,
		const string &value)
{
	mAssemblerAttrs[name] = value;
}

void 	
DataProcCreateDatasetDoc::setRecord(const boost::shared_ptr<DataRecord> record)
{
	mDataRecord = record;
}

void 	
DataProcCreateDatasetDoc::setRecordAttr(
		const string &name,
		const string &value)
{
	mRecordAttrs[name] = value;
}


void 	
DataProcCreateDatasetDoc::setCompareFunAttr(
		const string &key,
		const string &value)
{
	mCompareFunAttrs[key] = value;
}


string
DataProcCreateDatasetDoc::getConfig()
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

	if (mDataRecord)
		config += mDataRecord->getConfig();

	config += "</asm>";

	// 4. CompareFun node
	config += "<CompareFun ";
	for (map<string, string>::iterator itr=mCompareFunAttrs.begin(); 
			itr!=mCompareFunAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	config += "<cmp_fields>";
	for ( u32 i = 0; i < mCmpFieldAttrs.size(); i++)
	{
		config += "<field";
		config += " cmp_size=\"" + mCmpFieldAttrs[i].size + "\"";
		config += " cmp_datatype=\"" + mCmpFieldAttrs[i].data_type + "\"";
		      config += " cmp_pos=\"" + mCmpFieldAttrs[i].pos + "\"";
		config += " ></field>";
	}
	config += "</cmp_fields>";

	config +="</CompareFun>";

	config += "</datacollector>";
	config += "</dataproc>";
	return config;
}
