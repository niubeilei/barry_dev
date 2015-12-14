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

#include "Vector2DUtil/MeasureValueProcer.h"

#include "Rundata/Rundata.h"
#include "Debug/Debug.h"

AosMeasureValueProcer::AosMeasureValueProcer()
{
}


AosMeasureValueProcer::~AosMeasureValueProcer()
{
}


OmnString
AosMeasureValueProcer::toXmlString(const AosRundataPtr &rdata)
{
	OmnString str = "<m_value_procer>";
	str << mValueDef.toXmlString(rdata);
	
	str << "<procers>";
	for(u32 i=0; i<mProcers.size(); i++)
	{
		str << "<entry idx=\"" << mProcers[i].mIdx << "\" "
			<< "aggr_func_str=\"" << mProcers[i].mAggrFuncRaw->getAggrFuncName() << "\" "
			<< "></entry>";
	}
	str << "</procers>";

	str << "</m_value_procer>";
	return str;
}


bool
AosMeasureValueProcer::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	
	AosXmlTagPtr value_def_conf = conf->getFirstChild("m_value_def", true);
	aos_assert_r(value_def_conf, false);
	bool rslt = mValueDef.config(rdata, value_def_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr procers_conf = conf->getFirstChild("procers", true);
	aos_assert_r(procers_conf, false);
	
	Entry entry;
	OmnString aggr_str;
	AosXmlTagPtr entry_conf = procers_conf->getFirstChild(true);
	while(entry_conf)
	{
		entry.mIdx = entry_conf->getAttrU32("idx", 0);
		
		aggr_str = entry_conf->getAttrStr("aggr_func_str", "");	
		aos_assert_r(aggr_str != "", false);
		entry.mAggrFunc = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggr_str);
		aos_assert_r(entry.mAggrFunc, false);
		entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();

		mProcers.push_back(entry);
		entry_conf = procers_conf->getNextChild();
	}
	return true;
}


bool
AosMeasureValueProcer::add(
		const int idx,
		const AosAggrFuncObjPtr &aggr_func)
{
	aos_assert_r(idx >=0 && (u32)idx < mValueDef.mEntrys.size(), false);
	aos_assert_r(aggr_func, false);
	
	Entry entry;
	entry.mIdx = idx;
	entry.mAggrFunc = aggr_func;
	entry.mAggrFuncRaw = entry.mAggrFunc.getPtr();
	mProcers.push_back(entry);
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosMeasureValueProcer::updateOutputData(
		AosRundata *rdata,
		char * input_data,
		const u32 input_len,
		char * output_data,
		const u32 output_len)
{
	aos_assert_r(input_len == mValueDef.mValueSize, false);
	aos_assert_r(output_len == mValueDef.mValueSize, false);
	
	bool rslt, is_valid;
	char * input_value = 0, *output_value = 0;
	u32 input_value_len = 0, output_value_len = 0;
	//AosDataType::E data_type;
	AosDataType::E input_value_type, output_value_type;
	for(u32 i=0; i<mProcers.size(); i++)
	{
		rslt = mValueDef.getValue(input_data, input_len, mProcers[i].mIdx,
				input_value, input_value_type, input_value_len, is_valid);
		aos_assert_r(rslt, false);
		if(!is_valid) continue;
		
		rslt = mValueDef.getValue(output_data, output_len, mProcers[i].mIdx,
				output_value, output_value_type, output_value_len, is_valid);
		aos_assert_r(rslt, false);
		
		if(!is_valid)
		{
			// now this output hasbeen valid.
			//mValueDef.setValueValid(output_data, output_len,
			//		mProcers[i].mIdx);
			
			mValueDef.setValue(output_data, output_len,
					mProcers[i].mIdx, input_value, input_value_len);
			continue;
		}
		
		//data_type = mValueDef.mEntrys[mProcers[i].mIdx].mDataType;
		//rslt = mProcers[i].mAggrFuncRaw->updateOutputData(rdata, data_type,
		//	input_value, input_value_len, output_value, output_value_len);
		rslt = mProcers[i].mAggrFuncRaw->updateOutputData(rdata, 
			input_value, input_value_type, output_value, output_value_type);
		aos_assert_rr(rslt, rdata, false);
	}

	//OmnTagFuncInfo << endl;
	return true;
}

/*
bool
AosMeasureValueProcer::serializeTo(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	bool rslt = mValueDef.serializeTo(rdata, buff);
	aos_assert_r(rslt, false);

	buff->setU32(mProcers.size());
	for(u32 i=0; i<mProcers.size(); i++)
	{
		buff->setInt(mProcers[i].mIdx);
		buff->setOmnStr(mProcers[i].mAggrFuncRaw->getAggrFuncName());
	}
	return true;
}


bool
AosMeasureValueProcer::serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	bool rslt = mValueDef.serializeFrom(rdata, buff);
	aos_assert_r(rslt, false);
	
	Entry p_entry;
	u32 proc_num = buff->getU32(0);
	OmnString aggr_func_str;
	for(u32 i=0; i< proc_num; i++)
	{
		p_entry.mIdx = buff->getInt(-1);
		aggr_func_str = buff->getOmnStr("");
		aos_assert_r(aggr_func_str != "", false);
		p_entry.mAggrFunc = AosAggrFuncObj::pickAggrFuncStatic(rdata, aggr_func_str);
		aos_assert_r(p_entry.mAggrFunc, false);
		p_entry.mAggrFuncRaw = p_entry.mAggrFunc.getPtr();
		
		mProcers.push_back(p_entry);
	}
	return true;
}
*/

