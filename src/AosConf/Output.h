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
#ifndef Aos_AosConf_Output_h
#define Aos_AosConf_Output_h

#include "AosConf/DataRecord.h"
#include "AosConf/AosConfig.h"
#include "AosConf/DataAssembler.h"
#include "DataAssembler/DataAssembler.h"
#include "SEInterfaces/DataRecordObj.h"
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

namespace AosConf
{
class Output: public AosConf::AosConfBase
{
	struct CmpFun
	{
		string type;
		string reserve;
		string record_type;
	};

	struct CmpField
	{
		string name;
		string reserve;
	};

	struct AggrField
	{
		string name;
		string opr;
	};
protected:
	string										mName;	
	AosDataRecordType::E						mRecordType;
	boost::shared_ptr<DataRecord>				mDataRecord;
	boost::shared_ptr<DataAssembler>			mDataAssembler;
	AosDataRecordObjPtr							mRecord;
	AosDataAssemblerObjPtr						mAssembler;

	vector<CmpField>							mCmpFields;
	vector<AggrField>							mAggrFields;
	map<string, string>							mDataColAttrs;
	CmpFun										mCmpFun;

	string 										mDataCollectorType;
	map<string, string>							mShuffAttrs;

public:
	//name: output name
	Output(const string &name, const AosDataRecordType::E type);
	~Output() {};

	string	getConfig();
	void setField(
			const string &name, 
			const AosDataFieldType::E &field_type, 
			const int length);

	void setShuffAttr(
			const string &name, 
			const string &value);

	void setCmpFun(
			const string &fun_type,
			const string &cmpfun_reserve = "false");

	void setCmpField(
			const string &name,
			const string &cmp_reserve = "false");

	void  setDataColAttr(
			const string &name,
			const string &value);

	void setAggrField(
			const string &name,
			const string &aggrtype);

	void setAssemblerAttr(
			const string &name,
			const string &value);

	void init(
			const u64 task_docid,
			const AosRundataPtr &rdata);

	void setRecordConfig(const AosXmlTagPtr &xml);
	
	AosXmlTagPtr init2(
			const AosTaskObjPtr &task,
			const AosRundataPtr &rdata);

	AosDataRecordObjPtr getRecord() { return mRecord; }

	AosDataAssemblerObjPtr getAssembler() { return mAssembler; }
private:
	string 	convert(const string &type);
	void	getField(const string &name, u32 &i, boost::shared_ptr<DataField> &field);

};
}

#endif



