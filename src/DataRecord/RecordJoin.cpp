////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/03/31 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordJoin.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"


AosRecordJoin::AosRecordJoin(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eJoin,
	AOSRECORDTYPE_XML, flag AosMemoryCheckerFileLine),
mJoinType(eInnerJoin)
{
}


AosRecordJoin::AosRecordJoin(const JoinType type AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eJoin,
	AOSRECORDTYPE_XML, false AosMemoryCheckerFileLine),
mJoinType(type)
{
}


AosRecordJoin::AosRecordJoin(
		const AosRecordJoin &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mJoinType(rhs.mJoinType)
{
}


AosRecordJoin::~AosRecordJoin()
{
}


bool
AosRecordJoin::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	bool rslt = AosDataRecord::config(def, rdata);
	aos_assert_r(rslt, false);

	OmnString type = def->getAttrStr("join_type");
	if (type == "inner_join")
	{
		mJoinType = eInnerJoin;
	}
	else if (type == "out_join")
	{
		mJoinType = eOutJoin;
	}
	else
	{
		OmnAlarm << "error" << enderr;
		return false;
	}

	return true;
}


AosDataRecordObjPtr 
AosRecordJoin::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordJoin(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordJoin::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordJoin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordJoin::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordJoin * record = OmnNew AosRecordJoin(false AosMemoryCheckerFileLine);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordJoin::getFieldValue(
		const OmnString &field_name,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(field_name != "", false);

	int idx = field_name.find('.', false);
	aos_assert_r(idx > 0, false);

	OmnString r_name = field_name.substr(0, idx-1); 
	OmnString f_name = field_name.substr(idx+1, field_name.length()-1); 
	aos_assert_r(r_name != "" && f_name != "", false);

	map<OmnString, AosDataRecordObjPtr>::iterator itr = mRecords.find(r_name);
	if (itr == mRecords.end())
	{
		aos_assert_r(mJoinType == eOutJoin, false);

		value.setStr("");
		return true;
	}

	AosDataRecordObjPtr record = itr->second;
	aos_assert_r(record, false);

	return record->getFieldValue(f_name, value, copy_flag, rdata);
}
	

bool
AosRecordJoin::setRecords(
		vector<AosDataRecordObjPtr> &records,
		AosRundata *rdata)
{
	aos_assert_r(records.size() > 0, false);

	mRecords.clear();
	OmnString name;
	AosDataRecordObjPtr record;
	for (u32 i=0; i<records.size(); i++)
	{
		record = records[i];
		name = record->getRecordName();
		aos_assert_r(name != "", false);
		aos_assert_r(mRecords.count(name) == 0, false);
	
		mRecords[name] = record;
	}

	return true;
}


void
AosRecordJoin::clear()
{
	AosDataRecord::clear();
	mRecords.clear();
}

