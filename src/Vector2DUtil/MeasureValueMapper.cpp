////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "Vector2DUtil/MeasureValueMapper.h"

#include "Rundata/Rundata.h"
#include "Debug/Debug.h"



AosMeasureValueMapper::AosMeasureValueMapper()
{
}


AosMeasureValueMapper::~AosMeasureValueMapper()
{
}


OmnString
AosMeasureValueMapper::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<m_value_mapper>";
	str << mInputDef.toXmlString(rdata);
	str << mOutputDef.toXmlString(rdata);

	str << "<map_entrys>";
	for(u32 i=0; i<mMapEntrys.size(); i++)
	{
		str << "<entry input_idx=\"" << mMapEntrys[i].mInputIdx<< "\" "
			<< "output_idx=\"" << mMapEntrys[i].mOutputIdx << "\" "
			<< "aggr_func_str=\"" << mMapEntrys[i].mAggrFuncRaw->getAggrFuncName() << "\" "
			<< "></entry>";
	}
	str << "</map_entrys>";

	str << "</m_value_mapper>";
	return str;
}


bool
AosMeasureValueMapper::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	
	AosXmlTagPtr value_def_conf = conf->getFirstChild("m_value_def", true);
	aos_assert_r(value_def_conf, false);
	bool rslt = mInputDef.config(rdata, value_def_conf);
	aos_assert_r(rslt, false);
	
	value_def_conf = conf->getNextChild("m_value_def");
	aos_assert_r(value_def_conf, false);
	rslt = mOutputDef.config(rdata, value_def_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr map_entrys_conf = conf->getFirstChild("map_entrys", true);
	aos_assert_r(map_entrys_conf, false);
	
	Entry entry;
	OmnString aggr_str;
	AosXmlTagPtr entry_conf = map_entrys_conf->getFirstChild(true);
	while(entry_conf)
	{
		entry.mInputIdx = entry_conf->getAttrU32("input_idx", 0);
		entry.mOutputIdx = entry_conf->getAttrU32("output_idx", 0);
		
		aggr_str = entry_conf->getAttrStr("aggr_func_str", "");	
		aos_assert_r(aggr_str != "", false);
		entry.mAggrFunc = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggr_str);
		aos_assert_r(entry.mAggrFunc, false);
		entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();

		mMapEntrys.push_back(entry);
		entry_conf = map_entrys_conf->getNextChild();
	}
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosMeasureValueMapper::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	if(mRecordType != AosDataRecordType::eBuff)
	{
		bool rslt = mInputDef.serializeTo(buff);
		aos_assert_r(rslt, false);

		rslt = mOutputDef.serializeTo(buff);
		aos_assert_r(rslt, false);

		buff->setU32(mMapEntrys.size());
		for(u32 i=0; i<mMapEntrys.size(); i++)
		{
			buff->setInt(mMapEntrys[i].mInputIdx);
			buff->setInt(mMapEntrys[i].mOutputIdx);
			buff->setOmnStr(mMapEntrys[i].mAggrFuncRaw->getAggrFuncName());
		}
		OmnTagFuncInfo << endl;
	}
	else
	{
		buff->setU32(mAggrFuncs.size());
		for(u32 i=0; i<mAggrFuncs.size(); i++)
			buff->setOmnStr(mAggrFuncs[i]->getAggrFuncName());
		bool rslt = mOutputDef.serializeTo(buff);
		aos_assert_r(rslt, false);
	}
	return true;
}
	

bool
AosMeasureValueMapper::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	if(mRecordType != AosDataRecordType::eBuff)
	{
		bool rslt = mInputDef.serializeFrom(buff);
		aos_assert_r(rslt, false);

		rslt = mOutputDef.serializeFrom(buff);
		aos_assert_r(rslt, false);

		Entry entry;
		OmnString aggr_str;
		u32 entry_num = buff->getU32(0);
		for(u32 i=0; i<entry_num; i++)
		{
			entry.mInputIdx = buff->getInt(-1);
			entry.mOutputIdx = buff->getInt(-1);

			aggr_str = buff->getOmnStr("");
			aos_assert_r(aggr_str != "", false);
			entry.mAggrFunc = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggr_str);
			aos_assert_r(entry.mAggrFunc, false);
			entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();

			mMapEntrys.push_back(entry);
		}
		OmnTagFuncInfo << endl;
	}
	else
	{
		u32 entry_num = buff->getU32(0);
		for(u32 i=0; i<entry_num; i++)
		{
			OmnString aggr_str;
			aggr_str = buff->getOmnStr("");
			aos_assert_r(aggr_str != "", false);
			AosAggrFuncObjPtr aggr = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggr_str);
			mAggrFuncs.push_back(aggr);
		}
		bool rslt = mOutputDef.serializeFrom(buff);
		aos_assert_r(rslt, false);
	}
	return true;	
}


bool
AosMeasureValueMapper::add(
		const int input_idx,
		const int output_idx,
		const AosAggrFuncObjPtr &aggr_func)
{
	aos_assert_r(input_idx >=0 && 
			(u32)input_idx < mInputDef.mEntrys.size(), false);
	aos_assert_r(output_idx >=0 && 
			(u32)output_idx < mOutputDef.mEntrys.size(), false);
	aos_assert_r(aggr_func, false);

	Entry entry;
	entry.mInputIdx = input_idx;
	entry.mOutputIdx = output_idx;
	entry.mAggrFunc = aggr_func;
	entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();
	mMapEntrys.push_back(entry);

	//yang
	mAggrFuncs.push_back(entry.mAggrFuncRaw);

	OmnTagFuncInfo << endl;
	return true;
}

bool
AosMeasureValueMapper::addIndex(
		const int input_idx,
		const int output_idx,
		const AosAggrFuncObjPtr &aggr_func)
{
	aos_assert_r(input_idx >=0, false);
	aos_assert_r(output_idx >=0, false);
	aos_assert_r(aggr_func, false);

	Entry entry;
	entry.mInputIdx = input_idx;
	entry.mOutputIdx = output_idx;
	entry.mAggrFunc = aggr_func;
	entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();
	mMapEntrys.push_back(entry);
	OmnTagFuncInfo << endl;
	return true;
}

bool
AosMeasureValueMapper::getOutputData(
		AosRundata *rdata,
		char * input_data,
		const u32 input_len,
		char * output_data,
		const u32 output_len,
		bool &valid)
{
	aos_assert_r(input_len == mInputDef.mValueSize, false);
	aos_assert_r(output_len == mOutputDef.mValueSize, false);
	
	memset(output_data, 0, output_len);
	valid = false;

	bool rslt;
	bool crt_value_valid;
	char * input_value = 0;
	AosDataType::E value_type;
	u32 value_len;
	for(u32 i=0; i<mMapEntrys.size(); i++)
	{
		rslt = mInputDef.getValue(input_data, input_len, mMapEntrys[i].mInputIdx,
				input_value, value_type, value_len, crt_value_valid);
		aos_assert_r(rslt, false);
		if(!crt_value_valid) continue;
		
		valid = true;
		rslt = mOutputDef.setValue(output_data, output_len, mMapEntrys[i].mOutputIdx,
				input_value, value_type);
		aos_assert_r(rslt, false);
	}
	//OmnTagFuncInfo << endl;
	return true;
}


bool
AosMeasureValueMapper::updateOutputData(
		AosRundata *rdata,
		char * input_data,
		const u32 input_len,
		char * output_data,
		const u32 output_len)
{
	
	if(mRecordType == AosDataRecordType::eBuff)
	{
		aos_assert_r(output_len == mOutputDef.mValueSize, false);
		
		bool rslt, is_valid;
		char * input_value = 0, *output_value = 0;
		u32 input_value_len = 0, output_value_len = 0;
		AosDataType::E input_value_type, output_value_type;

		input_value = input_data;
		for(u32 i=0; i<mOutputDef.getValueNum(); i++)
		{
			//iterate all measures
			//yang
			//if(mMapEntrys[i].mAggrFunc->getAggrFuncName()=="dist_count")

			input_value_len = 8;

			rslt = mOutputDef.getValue(output_data, output_len, i,
					output_value, output_value_type, output_value_len, is_valid);
			aos_assert_r(rslt, false);

			if(!is_valid && mOpr== "insert")
			{
				//mOutputDef.setValueValid(output_data, output_len,
				//		mMapEntrys[i].mOutputIdx);

				mOutputDef.setValue(output_data, output_len,
						i, input_value, input_value_len);
				input_value += 8;
				continue;
			}

			//data_type = mOutputDef.mEntrys[mMapEntrys[i].mOutputIdx].mDataType;
			//rslt = mMapEntrys[i].mAggrFuncRaw->updateOutputData(rdata, data_type,
			//	input_value, input_value_len, output_value, output_value_len);
			if(mOpr == "insert" )
				rslt = mAggrFuncs[i]->updateOutputDataWithDeltaData(rdata,
					input_value, output_value_type, output_value, output_value_type,
					AosAggrFuncObj::INSERT);
			else if(mOpr == "delete")
				rslt = mAggrFuncs[i]->updateOutputDataWithDeltaData(rdata,
					input_value, output_value_type, output_value, output_value_type,
					AosAggrFuncObj::DELETE);

			input_value += 8;
		}
	}
	else
	{
		aos_assert_r(input_len == mInputDef.mValueSize, false);
		aos_assert_r(output_len == mOutputDef.mValueSize, false);
		
		bool rslt, is_valid;
		char * input_value = 0, *output_value = 0;
		u32 input_value_len = 0, output_value_len = 0;
		AosDataType::E input_value_type, output_value_type;
		for(u32 i=0; i<mMapEntrys.size(); i++)
		{
			//iterate all measures
			//yang
			//if(mMapEntrys[i].mAggrFunc->getAggrFuncName()=="dist_count")

			rslt = mInputDef.getValue(input_data, input_len, mMapEntrys[i].mInputIdx,
					input_value, input_value_type, input_value_len, is_valid);
			aos_assert_r(rslt, false);
			if(!is_valid) continue;

			rslt = mOutputDef.getValue(output_data, output_len, mMapEntrys[i].mOutputIdx,
					output_value, output_value_type, output_value_len, is_valid);
			aos_assert_r(rslt, false);

			if(!is_valid && mOpr == "insert")
			{
				//mOutputDef.setValueValid(output_data, output_len,
				//		mMapEntrys[i].mOutputIdx);

				mOutputDef.setValue(output_data, output_len,
						mMapEntrys[i].mOutputIdx, input_value, input_value_len);
				continue;
			}
			
			//data_type = mOutputDef.mEntrys[mMapEntrys[i].mOutputIdx].mDataType;
			//rslt = mMapEntrys[i].mAggrFuncRaw->updateOutputData(rdata, data_type,
			//	input_value, input_value_len, output_value, output_value_len);
//			rslt = mMapEntrys[i].mAggrFuncRaw->updateOutputData(rdata,
//				input_value, input_value_type, output_value, output_value_type);
//			aos_assert_rr(rslt, rdata, false);

			if(mOpr == "insert" || mOpr == "")
				rslt = mMapEntrys[i].mAggrFuncRaw->updateOutputDataWithDeltaData(rdata,
					input_value, output_value_type, output_value, output_value_type,
					AosAggrFuncObj::INSERT);
			else if(mOpr == "delete")
				rslt = mMapEntrys[i].mAggrFuncRaw->updateOutputDataWithDeltaData(rdata,
					input_value, output_value_type, output_value, output_value_type,
					AosAggrFuncObj::DELETE);

		}
	}
	//OmnTagFuncInfo << endl;
	return true;
}


