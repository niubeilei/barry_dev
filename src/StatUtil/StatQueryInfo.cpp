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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/Statistic.h"
#include "JQLStatement/JqlHaving.h"
#include "API/AosApi.h"
AosStatQueryInfo::AosStatQueryInfo()
:
mNeedFullScan(false),
mPageSize(0),
mPageOffset(0),
mHitShuffleFields(false),
mGrpbyTimeUnit(AosStatTimeUnit::eInvalid)
{
}


/*
bool
AosStatQueryInfo::init(
		const AosRundataPtr &rdata,
		const AosStatisticPtr &stat,
		//vector<AosStatMeasureField> &qry_measure_fields,
		vector<AosStatFieldPtr> &select_fields,
		vector<AosStatCond> &stat_conds,
		vector<AosStatFieldPtr> &grpby_fields,
		vector<AosStatFieldPtr> &orderby_fields,
		vector<bool> &order_ascs)
{
	aos_assert_r(stat, false);
	bool rslt;

	mStatName = stat->getStatName(); 
	mStatKeyFields = stat->getKeyFields();
	mTimeKeyField = stat->getTimeFieldName(); 
	mPageSize = 20;	// Tmp.

	//mQryMeasures = qry_measure_fields;
	//mQryConds = stat_conds;
	mGrpbyFields = grpby_fields;
	mOrderbyFields = orderby_fields;
	mOrderbyAscs = order_ascs;
	
	for(u32 i=0; i<select_fields.size(); i++)
	{
		if(select_fields[i]->getFieldType() == AosStatField::eMeasureField)	
		{
			mQryMeasures.push_back(select_fields[i]);
		}
	}
	
	for(u32 i=0; i<orderby_fields.size(); i++)
	{
		if(orderby_fields[i]->getFieldType() == AosStatField::eMeasureField)	
		{
			mQryMeasures.push_back(orderby_fields[i]);
		}
	}
	
	for(u32 i=0; i<grpby_fields.size(); i++)
	{
		if(grpby_fields[i]->getFieldType() == AosStatField::eTimeField)
		{
			AosStatTimeField *time_field =(AosStatTimeField *)grpby_fields[i].getPtr();
			mGrpbyTimeUnit = time_field->mTimeUnit;
		}
	}
	if(mGrpbyTimeUnit == AosStatTimeUnit::eInvalid && mTimeKeyField != "")
	{
		mGrpbyTimeUnit = AosStatTimeUnit::eAllTime;
	}

	AosStatFieldPtr cond_field;
	for(u32 i=0; i<stat_conds.size(); i++)
	{
		cond_field = stat_conds[i].mStatField;
		if(cond_field->getFieldType() == AosStatField::eTimeField)
		{
			rslt = AosStatTimeArea::parseTimeCond(rdata,
					stat_conds[i].mCondOpr, stat_conds[i].mCondValue1, mQryTimeConds);
			aos_assert_r(rslt, false);
			continue;
		}
		
		// measure cond not support yet.
		//if(stat->isMeasureRawField(cond_field->mName))
		//{
		//	cond_field->mName = generateMeasureKeyCondFname(rdata, stat,
		//		cond_field->mName, AosOpr_toStr(stat_conds[i].mCondOpr),
		//		stat_conds[i].mCondValue1);
		//}
		//aos_assert_r(cond_field->mName != "", false);
		mQryKeyConds.push_back(stat_conds[i]);
	}
	
	return true;
}
*/


bool
AosStatQueryInfo::init(
		const AosRundataPtr &rdata,
		const u32	page_size,	
		const u64 	page_offset,
		vector<AosStatCond> &stat_conds,
		vector<AosStatFieldPtr> &grpby_fields,
		vector<AosStatFieldPtr> &cube_grpby_fields,
		vector<vector<AosStatFieldPtr> > &rollup_grpby_fields_grp,
		vector<AosStatFieldPtr> &orderby_fields,
		vector<bool> &order_ascs,
		const OmnString &having_conds,
		const bool hitShuffleFields)
{
	bool rslt;
	
	mPageSize = page_size;
	mPageOffset = page_offset;
	mGrpbyFields = grpby_fields;
	mCubeGrpbyFields = cube_grpby_fields;
	mRollupGrpbyFieldsGrp = rollup_grpby_fields_grp;
	mOrderbyFields = orderby_fields;
	mOrderbyAscs = order_ascs;
	mHavingCond = having_conds;
	mHitShuffleFields = hitShuffleFields;

	for(u32 i=0; i<grpby_fields.size(); i++)
	{
		if(grpby_fields[i]->getFieldType() == AosStatField::eTimeField)
		{
			AosStatTimeField *time_field =(AosStatTimeField *)grpby_fields[i].getPtr();
			mGrpbyTimeUnit = time_field->mTimeUnit;
		}
	}
	if(mGrpbyTimeUnit == AosStatTimeUnit::eInvalid && mTimeKeyField != "")
	{
		mGrpbyTimeUnit = AosStatTimeUnit::eAllTime;
	}

	AosStatFieldPtr cond_field;
	for(u32 i=0; i<stat_conds.size(); i++)
	{
		cond_field = stat_conds[i].mStatField;
		if(cond_field->getFieldType() == AosStatField::eTimeField)
		{
			AosStatTimeUnit::E time_unit = cond_field->getTimeUnit();
			mStatTimeUnit = time_unit;

			rslt = AosStatTimeArea::parseTimeCond(rdata,time_unit,
					stat_conds[i].mCondOpr, stat_conds[i].mCondValue1, mQryTimeConds);
			aos_assert_r(rslt, false);
			continue;
		}
		
		// measure cond not support yet.
		//if(stat->isMeasureRawField(cond_field->mName))
		//{
		//	cond_field->mName = generateMeasureKeyCondFname(rdata, stat,
		//		cond_field->mName, AosOpr_toStr(stat_conds[i].mCondOpr),
		//		stat_conds[i].mCondValue1);
		//}
		//aos_assert_r(cond_field->mName != "", false);
		mQryKeyConds.push_back(stat_conds[i]);
	}
	
	return true;
}
	

bool
AosStatQueryInfo::initStat(
		const AosRundataPtr &rdata,
		const AosStatisticPtr &stat)
{
	mStatName = stat->getStatName(); 
	mStatKeyFields = stat->getKeyFields();
	mTimeKeyField = stat->getTimeFieldName(); 
	
	return true;
}


bool
AosStatQueryInfo::addQryMeasure(const AosStatFieldPtr &field)
{
	aos_assert_r(field && field->getFieldType() == AosStatField::eMeasureField, false);
	mQryMeasures.push_back(field);

	AosStatMeasureField * m_field = dynamic_cast<AosStatMeasureField *>(field.getPtr());
	if(m_field->mAggrFuncStr != "dist_count")	return true;		
	
	// to make_sure dist_count's key is in the mStatKeyFields's end.
	aos_assert_r(mStatKeyFields.size(), false);
	OmnString	mRawFname;
	for(u32 i= 0; i<mStatKeyFields.size(); i++)
	{
		if(mStatKeyFields[i] == m_field->mRawFname)
		{
			OmnString last_key = mStatKeyFields[mStatKeyFields.size() - 1];
			mStatKeyFields[mStatKeyFields.size() - 1] = mStatKeyFields[i];
			mStatKeyFields[i] = last_key;
			break;
		}
	}
	return true;
}


bool
AosStatQueryInfo::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	bool rslt;
	
	AosXmlTagPtr stat_conf = conf->getFirstChild("stat_base", true);
	aos_assert_r(stat_conf, false);
	mStatName = stat_conf->getAttrStr("stat_name", "");
	aos_assert_r(mStatName != "", false);
	mPageSize = stat_conf->getAttrU32("page_size", 0); 
	mPageOffset = stat_conf->getAttrU64("page_offset", 0);
	//mHavingCond = stat_conf->getAttrStr("having_conds", "");
	
	//aos_assert_r(mPageSize, false);		// maybe is 0.
	mNeedFullScan = stat_conf->getAttrBool("need_full_scan", false);

	// maybe no time_field
	mTimeKeyField = stat_conf->getAttrStr("time_field", "");
	//aos_assert_r(mTimeKeyField != "", false);

	OmnString fname;
	AosXmlTagPtr key_confs = conf->getFirstChild("stat_key_fields", false); 
	aos_assert_r(key_confs, 0);	
	AosXmlTagPtr k_conf = key_confs->getFirstChild(true);
	while(k_conf)
	{
		fname = k_conf->getAttrStr("field_name", "");
		aos_assert_r(fname != "", 0);
		mStatKeyFields.push_back(fname);
		
		k_conf = key_confs->getNextChild();
	}

	// config qry_measures field.
	AosXmlTagPtr qry_measure_confs = conf->getFirstChild("qry_measures", true);
	aos_assert_r(qry_measure_confs, false);
	AosXmlTagPtr m_conf = qry_measure_confs->getFirstChild(true);
	while(m_conf)
	{
		AosStatFieldPtr field = AosStatField::configStatic(m_conf);
		aos_assert_r(field && field->getFieldType() == AosStatField::eMeasureField, false);

		mQryMeasures.push_back(field);
		m_conf = qry_measure_confs->getNextChild();
	}
	
	// config qry_key_conds.
	AosXmlTagPtr qry_key_confs = conf->getFirstChild("qry_key_conds", true);
	if(qry_key_confs)
	{
		AosStatCond cond;
		AosXmlTagPtr k_conf = qry_key_confs->getFirstChild(true);
		while(k_conf)
		{
			rslt = cond.config(k_conf);
			aos_assert_r(rslt, false); 

			mQryKeyConds.push_back(cond);
			k_conf = qry_key_confs->getNextChild();
		}
	}

	// config qry_time_conds.
	AosXmlTagPtr qry_time_confs = conf->getFirstChild("qry_time_conds", true);
	if(qry_time_confs)
	{
		AosStatTimeArea time_area;
		AosXmlTagPtr t_conf = qry_time_confs->getFirstChild(true);
		while(t_conf)
		{
			rslt = time_area.config(t_conf);
			aos_assert_r(rslt, false); 

			mQryTimeConds.push_back(time_area);
			t_conf = qry_time_confs->getNextChild();
		}
	}

	// config grpby fields.
	AosXmlTagPtr grpby_confs = conf->getFirstChild("group_by_fields", true);
	//aos_assert_r(grpby_confs, false);
	if(grpby_confs)
	{
		k_conf = grpby_confs->getFirstChild(true);
		while(k_conf)
		{
			AosStatFieldPtr field = AosStatField::configStatic(k_conf);
			aos_assert_r(field, false);
			
			mGrpbyFields.push_back(field);	//Temp
			if(field->getFieldType() == AosStatField::eTimeField)
			{
				AosStatTimeField *time_field =(AosStatTimeField *)field.getPtr();
				mGrpbyTimeUnit = time_field->mTimeUnit;
			}
			k_conf = grpby_confs->getNextChild();
		}
	}
	if(mGrpbyTimeUnit == AosStatTimeUnit::eInvalid && mTimeKeyField != "")
	{
		mGrpbyTimeUnit = AosStatTimeUnit::eAllTime;
	}

	// config order by field.
	AosXmlTagPtr orderby_field_confs = conf->getFirstChild("orderby_fields");
	if(orderby_field_confs)
	{
		bool is_asc;
		AosXmlTagPtr f_conf;
		AosXmlTagPtr each_conf = orderby_field_confs->getFirstChild(true);
		while(each_conf)
		{
			is_asc = each_conf->getAttrBool("is_asc", false);
				
			f_conf = each_conf->getFirstChild(true);
			aos_assert_r(f_conf, false);
		
			AosStatFieldPtr field = AosStatField::configStatic(f_conf);
			aos_assert_r(field, false);
			
			mOrderbyFields.push_back(field);
			mOrderbyAscs.push_back(is_asc);
			each_conf = orderby_field_confs->getNextChild();
		}
	}

	//get Shuffle_fields
	AosXmlTagPtr shuffleField = conf->getFirstChild("hit_shuffle");
	if(shuffleField)
	{
		OmnString meetShuffleFields = shuffleField->getAttrStr("hit_shuffle_fields");
		if(meetShuffleFields == "true")
		mHitShuffleFields = true;
	}

	//get Having cond
	mHavingCond = "";
	AosXmlTagPtr havingNode = conf->getFirstChild("having_conds");	
	if (havingNode)
	{
		mHavingCond = havingNode->getAttrStr("zky_having_expr");
	}
	//arvin 2015.09.02
	AosXmlTagPtr statkey_fields = conf->getFirstChild("stat_key_fields");
	aos_assert_r(statkey_fields,false);
	AosXmlTagPtr statkey = statkey_fields->getFirstChild("field");
	int idx1 = mQryMeasures.size()+mGrpbyFields.size() ;
	int idx2 = 0; 
	AosDataType::E type;
	OmnString type_str,key;
	while(statkey)
	{
		key = statkey->getAttrStr("field_name","");
		type_str = statkey->getAttrStr("type","");
		type = AosDataType::toEnum(type_str);
		mKeyIdxMap[key]= idx1;
		mKeyIdxTypeMap[idx2] = type;
		idx1++;
		idx2++;
		statkey = statkey_fields->getNextChild();		
	}
	AosXmlTagPtr stat_base_doc = conf->getFirstChild("stat_base");
	OmnString time_field = stat_base_doc->getAttrStr("time_field","");
	if(time_field !="")
	{
		mKeyIdxMap[time_field]= idx1;
		mKeyIdxTypeMap[idx2] = AosDataType::eDateTime;
	}	
	return true;
}


bool
AosStatQueryInfo::toXmlString(OmnString &str)
{
	str << "<stat_base stat_name=\"" << mStatName << "\" "
		<< "page_size=\"" << mPageSize << "\" "
		<< "page_offset=\"" << mPageOffset << "\" "
		<< "need_full_scan=\"" << (mNeedFullScan ? "true" : "false") << "\" ";
	if(mTimeKeyField != "") str << "time_field=\"" << mTimeKeyField << "\" ";
	str	<< "></stat_base>";
	
	str << "<stat_key_fields>";
	for(u32 i=0; i<mStatKeyFields.size(); i++)
	{
		str << "<field field_name=\"" << mStatKeyFields[i] << "\" ></field>";
	}
	str << "</stat_key_fields>";

	str << "<qry_measures>";
	for(u32 i=0; i<mQryMeasures.size(); i++)
	{
		str << mQryMeasures[i]->toXmlString();
	}
	str << "</qry_measures>";
	
	if(mQryKeyConds.size())
	{
		str << "<qry_key_conds>";
		for(u32 i=0; i<mQryKeyConds.size(); i++)
		{
			str << mQryKeyConds[i].toXmlString();
		}
		str << "</qry_key_conds>";
	}

	if(mQryTimeConds.size())
	{
		str << "<qry_time_conds>";
		for(u32 i=0; i<mQryTimeConds.size(); i++)
		{
			str << mQryTimeConds[i].toXmlString(); 
		}
		str << "</qry_time_conds>";
	}

	//aos_assert_r(mGrpbyFields.size(), false);
	if(mGrpbyFields.size())
	{
		str << "<group_by_fields>";
		for(u32 i=0; i<mGrpbyFields.size(); i++)
		{
			str << mGrpbyFields[i]->toXmlString();
		}
		str << "</group_by_fields>";
	}

	if(mOrderbyFields.size())
	{
		str << "<orderby_fields>";
		for(u32 i=0; i<mOrderbyFields.size(); i++)
		{
			str << "<orderby_field "
				<< "is_asc=\"" << (mOrderbyAscs[i] ? "true":"false") << "\" >" 
				<< mOrderbyFields[i]->toXmlString()
				<< "</orderby_field>";
		}
		
		str << "</orderby_fields>";
	}
	OmnString hitShuffleFields = "false";
	if(mHitShuffleFields)
	{
		hitShuffleFields = "true";
		str << "<hit_shuffle hit_shuffle_fields=\"" << hitShuffleFields << "\" >"
			<< "</hit_shuffle>";
	}

	if (mHavingCond != "")
	{

		str << "<having_conds zky_having_expr=\"" << mHavingCond << "\" >"
		    << "</having_conds>";
	}

	return true;
}


OmnString
AosStatQueryInfo::toXmlString()
{
	OmnString str = "<stat_qry>";
	
	bool rslt = toXmlString(str);
	aos_assert_r(rslt, "");

	str << "</stat_qry>";
	return str;
}


/*
bool
AosStatQueryInfo::serializeTo(const AosBuffPtr &buff)
{
	buff->setU32(mQryMeasures.size());
	for(u32 i=0; i<mQryMeasures.size(); i++)
	{
		buff->setOmnStr(mQryMeasures[i].mName);
		buff->setOmnStr(mQryMeasures[i].mAggrFuncStr);
		buff->setOmnStr(mQryMeasures[i].mAggrFuncStr);
	}
	
	buff->setU32(mQryTimeArea.size());
	for(u32 i=0; i<mQryTimeArea.size(); i++)
	{
		mQryTimeArea[i].serializeTo(buff);
	}
	
	buff->setU32(mGrpbyKeyFields.size());
	for(u32 i=0; i<mGrpbyKeyFields.size(); i++)
	{
		buff->setOmnStr(mGrpbyKeyFields[i]);
	}
	
	buff->setU32(mGrpbyTimeUnit);
	return true;
}


bool
AosStatQueryInfo::serializeFrom(const AosBuffPtr &buff)
{
	u32 measure_num = buff->getU32(0);
	QueryMeasureInfo measure_info;
	for(u32 i=0; i< measure_num; i++)
	{
		measure_info.mName = buff->getOmnStr("");
		aos_assert_r(measure_info.mName != "", false);
		
		measure_info.mAggrFuncStr = buff->getOmnStr("");
		aos_assert_r(measure_info.mAggrFuncStr != "", false);
		
		measure_info.mRawFieldName = buff->getOmnStr("");
		aos_assert_r(measure_info.mRawFieldName != "", false);
		
		mQryMeasures.push_back(measure_info);
	}
	
	u32 time_areas_num = buff->getU32(0);	
	AosStatTimeArea time_area;
	for(u32 i=0; i< time_areas_num; i++)
	{
		time_area.serializeFrom(buff);	
		mQryTimeArea.push_back(time_area);
	}
	
	u32 grpby_key_num = buff->getU32(0);
	OmnString key_name;
	for(u32 i=0; i< grpby_key_num; i++)
	{
		key_name = buff->getOmnStr("");
		aos_assert_r(key_name != "", false);
		
		mGrpbyKeyFields.push_back(key_name);
	}
	mGrpbyTimeUnit = (AosStatTimeUnit::E)buff->getU32(0);	
	return true;
}
*/
			

