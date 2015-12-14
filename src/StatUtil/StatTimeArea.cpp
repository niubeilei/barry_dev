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

#include "StatUtil/StatTimeArea.h"

	
AosStatTimeArea::AosStatTimeArea()
:
start_time(-1),
end_time(-1),
time_unit(AosStatTimeUnit::eInvalid)
{
}


AosStatTimeArea::AosStatTimeArea(i64 s_time, i64 e_time, AosStatTimeUnit::E t_unit)
:
start_time(s_time),
end_time(e_time),
time_unit(t_unit)
{
}

	
OmnString
AosStatTimeArea::toXmlString()
{
	OmnString str = "<time ";
	str << "start_time=\"" << start_time << "\" "
		<< "end_time=\"" << end_time << "\" "
		<< "time_unit=\"" << AosStatTimeUnit::toStr(time_unit) << "\" "
		<< "/>";
	return str;
}

	
bool
AosStatTimeArea::config(const AosXmlTagPtr &conf)
{
	start_time = conf->getAttrInt("start_time", -1);
	end_time = conf->getAttrInt("end_time", -1);;
	aos_assert_r(start_time != -1 || end_time != -1, false);
	
	OmnString time_unit_str = conf->getAttrStr("time_unit", "");
	aos_assert_r(time_unit_str != "", false);
	time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str); 
	
	return true;
}


/*
bool
AosStatTimeArea::serializeTo(const AosBuffPtr &buff)
{
	buff->setI64(start_time);
	buff->setI64(end_time);
	buff->setU32(time_unit);
	return true;
}
	

bool
AosStatTimeArea::serializeFrom(const AosBuffPtr &buff)
{
	start_time = buff->getInt64(-1);
	end_time = buff->getInt64(-1);
	time_unit = (AosStatTimeUnit::E)buff->getU32(0);
	return true;
}
*/

bool
AosStatTimeArea::parseTimeCond(
		const AosRundataPtr &rdata,
		AosStatTimeUnit::E &time_unit,
		const AosOpr &cond_opr,
		const OmnString &time_str,
		vector<AosStatTimeArea> &time_areas)
{
	// Ketty Temp
	//str2EpochTime need time_str "%Y-%m-%d %H:%M:%S" ,it support str
	//i64	epoch_time = AosTimeUtil::str2EpochTime(time_str);
	
	i64 epoch_time = time_str.toInt64(0);

	if (epoch_time != -1)
	{
		epoch_time = AosStatTimeUnit::parseTimeValue(epoch_time, 
				AosStatTimeUnit::eEpochTime, time_unit);
	}

	bool rslt;
	AosStatTimeArea new_info;
	switch (cond_opr)
	{
	case eAosOpr_gt:
		 new_info.start_time = epoch_time + 1;
		 rslt = intersectTimeCondByAnd(time_areas, new_info);
		 aos_assert_r(rslt, false);
		 return true;

	case eAosOpr_ge:
		 new_info.start_time = epoch_time;
		 rslt = intersectTimeCondByAnd(time_areas, new_info);
		 aos_assert_r(rslt, false);
		 return true;

	case eAosOpr_lt:
		 new_info.end_time = epoch_time - 1;
		 rslt = intersectTimeCondByAnd(time_areas, new_info);
		 return true;

	case eAosOpr_le:
		 new_info.end_time = epoch_time;
		 rslt = intersectTimeCondByAnd(time_areas, new_info);
		 aos_assert_r(rslt, false);
		 return true;

	case eAosOpr_eq:
		 new_info.start_time = epoch_time;
		 new_info.end_time = epoch_time;
		 rslt = intersectTimeCondByAnd(time_areas, new_info);
		 aos_assert_r(rslt, false);
		 return true;

	case eAosOpr_ne:
	{
		vector<AosStatTimeArea>	or_conds;
		new_info.start_time = -1; 
		new_info.end_time = epoch_time - 1;
		or_conds.push_back(new_info);

		new_info.start_time = epoch_time + 1;
		new_info.end_time = -1;
		or_conds.push_back(new_info);
		
		rslt = intersectTimeCondByOr(time_areas, or_conds);
		return true;
	}

	default:
		 OmnNotImplementedYet;
		 return false;
	}
	return false;
}


bool
AosStatTimeArea::intersectTimeCondByAnd(
		vector<AosStatTimeArea> &rslt_conds,
		AosStatTimeArea &new_cond)
{
	if(rslt_conds.size() == 0)
	{
		// Ketty Temp
		new_cond.time_unit = AosStatTimeUnit::eEpochTime;
		//new_cond.time_unit = AosStatTimeUnit::eEpochDay;
		rslt_conds.push_back(new_cond);
		return true;
	}
	
	bool rslt = intersectTimeCondByAndPriv(rslt_conds, new_cond);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStatTimeArea::intersectTimeCondByOr(
		vector<AosStatTimeArea> &rslt_conds,
		vector<AosStatTimeArea> &or_conds)
{
	if(rslt_conds.size() == 0)
	{
		for(u32 i=0; i<or_conds.size(); i++)
		{
			// Ketty Temp
			or_conds[i].time_unit = AosStatTimeUnit::eEpochTime;
			//or_conds[i].time_unit = AosStatTimeUnit::eEpochDay;
			rslt_conds.push_back(or_conds[i]);
		}
		return true;
	}

	bool rslt;
	vector<AosStatTimeArea> orig_conds = rslt_conds;
	rslt_conds.clear();
	vector<AosStatTimeArea> crt_conds;

	for(u32 i=0; i<or_conds.size(); i++)
	{
		crt_conds = orig_conds;
		rslt = intersectTimeCondByAndPriv(crt_conds, or_conds[i]);
		aos_assert_r(rslt, false);
		
		for(u32 j=0; j<crt_conds.size(); j++)
		{
			rslt_conds.push_back(crt_conds[j]);
		}
	}
	return true;
}

//Wumeng, 2015-11-25 bug jimodb-1252
//Assume time_units of all the cond are same
bool
AosStatTimeArea::intersectTimeCondByAndPriv(
		vector<AosStatTimeArea> &rslt_conds,
		AosStatTimeArea &new_cond)
{
	AosStatTimeArea info;
	// Ketty Temp
	info.time_unit = AosStatTimeUnit::eEpochTime;
	//info.time_unit = new_cond.time_unit;
	vector<AosStatTimeArea> orig_cond = rslt_conds;
	rslt_conds.clear();
	
	int64_t rslt_start_time = -1, rslt_end_time = -1;
	for(u32 i=0; i<orig_cond.size(); i++)
	{
		AosStatTimeArea & crt_cond = orig_cond[i];
	
		if(new_cond.start_time != -1 && crt_cond.start_time != -1)
		{
			rslt_start_time = new_cond.start_time > crt_cond.start_time ? 
				new_cond.start_time : crt_cond.start_time;
		}
		else if (new_cond.start_time == -1 && crt_cond.start_time != -1)
		{
			rslt_start_time = crt_cond.start_time;
		}
		else if (new_cond.start_time != -1 && crt_cond.start_time == -1)
		{
			rslt_start_time = new_cond.start_time;
		}
		else
		{
			rslt_start_time =  -1;
		}
			
		//rslt_start_time = -1;
		//if(new_cond.start_time > crt_cond.start_time)
		//{
		//	rslt_start_time = new_cond.start_time;	
		//}
		
		if(new_cond.end_time != -1 && crt_cond.end_time != -1)
		{
			rslt_end_time = new_cond.end_time < crt_cond.end_time ?
				new_cond.end_time : crt_cond.end_time;
		}
		else if(new_cond.end_time == -1 && crt_cond.end_time != -1)
		{
			rslt_end_time = crt_cond.end_time;
		}
		else if(new_cond.end_time != -1 && crt_cond.end_time == -1)
		{
			rslt_end_time = new_cond.end_time;
		}
		else
		{
			rslt_end_time = -1;
		}
		//arvin 2015.08.13
		//JIMODB-335:we will check it in StatSvr
//		if(rslt_start_time > rslt_end_time)	continue;
	
		info.start_time = rslt_start_time;
		info.end_time = rslt_end_time;
		rslt_conds.push_back(info);
	}
	return true;	
}

