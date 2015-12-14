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
// 2014/08/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/Statistic.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "StatUtil/StatDefineDoc.h"
#include "StatUtil/StatInternalDoc.h"
#include "StatUtil/StatField.h"
#include "Debug/Debug.h"

AosStatistic::AosStatistic()
:
mStatId(0)
{
}


AosStatistic::~AosStatistic()
{
}


AosStatisticPtr
AosStatistic::retrieveStat(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_qry_conf)
{
	aos_assert_r(stat_qry_conf, 0);

	OmnString stat_objid = stat_qry_conf->getAttrStr("zky_stat_objid", "");
	aos_assert_r(stat_objid != "", 0);
	
	OmnString stat_identify_key = stat_qry_conf->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(stat_identify_key != "", 0);
	
	AosXmlTagPtr stat_doc = AosGetDocByObjid(stat_objid, rdata);
	aos_assert_r(stat_doc, 0); 
	
	AosXmlTagPtr inte_stat_conf = getInternalStatConf(rdata, stat_doc, stat_identify_key);

	aos_assert_r(inte_stat_conf, 0);
	AosStatisticPtr stat = OmnNew AosStatistic();
	bool rslt = stat->config(rdata, inte_stat_conf);
	aos_assert_r(rslt, 0);
	
	return stat;
}


AosXmlTagPtr
AosStatistic::getInternalStatConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_doc,
		const OmnString &stat_identify_key)
{
	aos_assert_r(stat_doc && stat_identify_key!="", 0);

	AosXmlTagPtr internal_stat_confs = stat_doc->getFirstChild("internal_statistics");
	//aos_assert_r(internal_stat_confs, 0);
	if(!internal_stat_confs)	return 0;

	OmnString crt_identify_key;
	AosXmlTagPtr crt_stat_conf = internal_stat_confs->getFirstChild(true);
	AosXmlTagPtr stat_conf;
	while(crt_stat_conf)
	{
		crt_identify_key = crt_stat_conf->getAttrStr("zky_stat_identify_key", "");	
		aos_assert_r(crt_identify_key != "", 0);
		if(crt_identify_key != stat_identify_key)
		{
			crt_stat_conf = internal_stat_confs->getNextChild();
			continue;
		}
		
		OmnString stat_conf_objid;
		stat_conf_objid = crt_stat_conf->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(stat_conf_objid != "", 0);
		
		stat_conf = AosGetDocByObjid(stat_conf_objid, rdata);
		aos_assert_r(stat_conf, 0);
		
		return stat_conf;
	}

	OmnTagFuncInfo << endl;
	return 0;
}


AosStatisticPtr
AosStatistic::retrieveStat(
		const AosRundataPtr &rdata,
		const OmnString &stat_name)
{
	aos_assert_r(stat_name != "", 0);
		
	AosXmlTagPtr inte_stat_conf = AosGetDocByObjid(stat_name, rdata);
	aos_assert_r(inte_stat_conf, 0);
	
	AosStatisticPtr stat = OmnNew AosStatistic();
	bool rslt = stat->config(rdata, inte_stat_conf);
	aos_assert_r(rslt, 0);
	
	return stat;
}



/*
AosStatisticPtr
AosStatistic::refactorStat(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_conf)
{
	aos_assert_r(stat_conf, 0);
	bool rslt;

	OmnString stat_objid = stat_conf->getAttrStr("zky_stat_objid", "");
	aos_assert_r(stat_objid != "", 0);
	
	OmnString stat_identify_key = stat_conf->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(stat_identify_key != "", 0);
	
	AosXmlTagPtr stat_doc = AosGetDocByObjid(stat_objid, rdata);
	aos_assert_r(stat_doc, 0); 
	
	AosXmlTagPtr inte_stat_conf = refactorInternalStatConf(rdata, 
			stat_doc, stat_name, stat_identify_key);
	aos_assert_r(inte_stat_conf, 0);
	
	AosStatisticPtr stat = OmnNew AosStatistic();
	rslt = stat->config(rdata, inte_stat_conf);
	aos_assert_r(rslt, 0);
	
	OmnTagFuncInfo << endl;
	return stat;
}
*/

/*
AosXmlTagPtr
AosStatistic::refactorInternalStatConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_doc,
		const OmnString &stat_identify_key)
{
	bool rslt;
	OmnString stat_name = stat_doc->getAttrStr("zky_stat_name");
	aos_assert_r(stat_name != "", 0);
	AosStatInternalDoc stat_intedoc(stat_identify_key);
	
	AosXmlTagPtr intedoc_conf = getInternalStatConf(rdata,
			stat_doc, stat_identify_key);
	rslt = stat_intedoc.config(rdata, intedoc_conf);
	aos_assert_r(rslt, 0);
	bool new_inte_doc = intedoc_conf ? false: true;
	
	AosStatDefineDoc stat_defdoc;
	AosXmlTagPtr stat_def_confs = stat_doc->getFirstChild("statistic_defs");
	aos_assert_r(stat_def_confs, 0);
	
	OmnString def_objid;
	AosXmlTagPtr defdoc_conf;
	AosXmlTagPtr each_conf = stat_def_confs->getFirstChild(true);
	while(each_conf)
	{
		def_objid = each_conf->getAttrStr("zky_stat_conf_objid", "");	
		aos_assert_r(def_objid != "", 0);
	
		defdoc_conf = AosGetDocByObjid(def_objid, rdata);
		aos_assert_r(defdoc_conf, 0); 
		
		rslt = stat_defdoc.config(defdoc_conf);
		aos_assert_r(rslt, 0);
		//felicia, 2014/09/18
		//if(stat_defdoc.getStatIdentifyKey() != stat_identify_key)	continue;
		if(stat_defdoc.getStatIdentifyKey() != stat_identify_key)
		{
			each_conf = stat_def_confs->getNextChild();
			continue;
		}
	
		rslt = stat_intedoc.addStatDefDoc(rdata, stat_defdoc);
		aos_assert_r(rslt, 0);	
		
		each_conf = stat_def_confs->getNextChild();
	}
	
	OmnString intedoc_conf_str = stat_intedoc.toXmlString(rdata);
	intedoc_conf = AosXmlParser::parse(intedoc_conf_str AosMemoryCheckerArgs);
	aos_assert_r(intedoc_conf, 0);
	if(!new_inte_doc)
	{
		rslt = AosModifyDoc(intedoc_conf, rdata);
		aos_assert_r(rslt, 0);
		return intedoc_conf;
	}
	
	rslt = addNewInternalDocConf(rdata, stat_doc, stat_name,
			stat_identify_key, intedoc_conf);
	aos_assert_r(rslt, 0);

	OmnTagFuncInfo << endl;
	return intedoc_conf;
}
*/

	
AosXmlTagPtr
AosStatistic::refactorInternalStatConf(
		const AosRundataPtr &rdata,
		//const AosXmlTagPtr &stat_doc,
		const OmnString &stat_doc_objid,
		const AosXmlTagPtr &stat_def_doc)
{
	bool rslt;
	AosXmlTagPtr stat_doc = AosGetDocByObjid(stat_doc_objid, rdata);
	aos_assert_r(stat_doc, 0); 
	
	//OmnString stat_name = stat_doc->getAttrStr("zky_stat_name");
	OmnString stat_name = stat_def_doc->getAttrStr("zky_stat_name");
	aos_assert_r(stat_name != "", 0);
	
	OmnString stat_identify_key = stat_def_doc->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(stat_identify_key != "", 0);
	AosStatInternalDoc stat_intedoc(stat_identify_key);
	
	bool new_inte_doc = true;
	AosXmlTagPtr intedoc_conf;
	/*
	AosXmlTagPtr intedoc_conf = getInternalStatConf(rdata,
			stat_doc, stat_identify_key);
	if(intedoc_conf)
	{
		rslt = stat_intedoc.config(rdata, intedoc_conf);
		aos_assert_r(rslt, 0);
	
		//new_inte_doc = false;
	}
	*/

	AosStatDefineDoc stat_defdoc;
	rslt = stat_defdoc.config(stat_def_doc);
	aos_assert_r(rslt, 0);

	rslt = stat_intedoc.addStatDefDoc(rdata, stat_defdoc);
	aos_assert_r(rslt, 0);	

	OmnString intedoc_conf_str = stat_intedoc.toXmlString(rdata);
	intedoc_conf = AosXmlParser::parse(intedoc_conf_str AosMemoryCheckerArgs);
	aos_assert_r(intedoc_conf, 0);
	if(!new_inte_doc)
	{
		rslt = AosModifyDoc(intedoc_conf, rdata);
		aos_assert_r(rslt, 0);
		return intedoc_conf;
	}
	
	rslt = addNewInternalDocConf(rdata, stat_doc, stat_name,
			stat_identify_key, intedoc_conf);
	aos_assert_r(rslt, 0);

	OmnTagFuncInfo << endl;
	return intedoc_conf;
}

bool
AosStatistic::addNewInternalDocConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_doc,
		const OmnString &stat_name,
		const OmnString &stat_identify_key,
		AosXmlTagPtr &new_intedoc_conf)
{
	aos_assert_r(new_intedoc_conf, false);	
	AosXmlTagPtr internal_stat_confs = stat_doc->getFirstChild("internal_statistics");
	if(!internal_stat_confs)
	{
		OmnString internal_confs_str = "<internal_statistics></internal_statistics>";
		internal_stat_confs = AosXmlParser::parse(internal_confs_str AosMemoryCheckerArgs);
		internal_stat_confs = stat_doc->addNode(internal_stat_confs);	
	}

	OmnString intedoc_objid = stat_name;
	intedoc_objid << "_internal_" << internal_stat_confs->getNumChilds();

	OmnString new_inte_tag_str = "<statistic zky_stat_identify_key=\"";
	new_inte_tag_str << stat_identify_key << "\" zky_stat_conf_objid=\""
		<< intedoc_objid <<"\"></statistic>";
	AosXmlTagPtr new_inte_tag = AosXmlParser::parse(new_inte_tag_str AosMemoryCheckerArgs);
	internal_stat_confs->addNode(new_inte_tag);	

	bool rslt = AosModifyDoc(stat_doc, rdata);
	aos_assert_r(rslt, false);
	
	new_intedoc_conf->setAttr("zky_stat_name", stat_name);
	new_intedoc_conf->setAttr(AOSTAG_OBJID, intedoc_objid);
	new_intedoc_conf->setAttr(AOSTAG_PUBLIC_DOC, "true");
	new_intedoc_conf->setAttr(AOSTAG_CTNR_PUBLIC, "true");

	new_intedoc_conf = AosCreateDoc(new_intedoc_conf->toString(), true, rdata);
	aos_assert_r(new_intedoc_conf, false);
	OmnString check_doc_objid = new_intedoc_conf->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(check_doc_objid == intedoc_objid, false);

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatistic::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	bool rslt;
	
	mStatId = conf->getAttrU64(AOSTAG_DOCID, 0); 
	aos_assert_r(mStatId, false);
	
	mStatName = conf->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(mStatName != "", false);

	mIdentifyKey = conf->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(mIdentifyKey != "", false);

	rslt = configTimeField(rdata, conf);
	aos_assert_r(rslt, false);

	rslt = configKeyFields(rdata, conf); 
	aos_assert_r(rslt, false);
	
	rslt = configVt2dInfos(rdata, conf);
	aos_assert_r(rslt, false);

	rslt = configShuffleFields(rdata, conf);
	aos_assert_r(rslt, false);
	
	mCubeConfs = conf->getFirstChild("stat_cubes", false); 
	aos_assert_r(mCubeConfs, false);	
	AosXmlTagPtr cube_conf = mCubeConfs->getFirstChild();
	while(cube_conf)
	{
		cube_conf->setAttr("stat_id", mStatId);
		cube_conf = mCubeConfs->getNextChild();
	}

	OmnTagFuncInfo << endl;
	return true;	
}


bool
AosStatistic::configTimeField(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)
{
	// <time_field time_unit="xxx" field_name="key_field8">
	aos_assert_r(conf, false);
	
	// maybe no has time_field
	AosXmlTagPtr time_conf = conf->getFirstChild("time_field", false); 
	//aos_assert_r(time_conf , false);
	if(!time_conf)	return true;

	mTimeField = time_conf->getAttrStr("time_field_name", "");
	if (mTimeField == "")
	{
		mTimeField = time_conf->getAttrStr("field_name", "");
		aos_assert_r(mTimeField != "", false);
	}
	//arvin 2015.08.12
	//JIMODB-378
	AosXmlTagPtr vt2ds_conf = conf->getFirstChild("vector2ds", false);
	if(!vt2ds_conf) return true;
	AosXmlTagPtr vt2d_conf = vt2ds_conf->getFirstChild("vt2d", false);
	if(!vt2d_conf) return true;
	AosXmlTagPtr vt2dInfo_conf = vt2d_conf->getFirstChild("vt2d_info", false);
	if(!vt2dInfo_conf) return true;
	mTimeUnit = vt2dInfo_conf->getAttrStr("grpby_time_unit","");
	return true;
}


bool
AosStatistic::configKeyFields(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)
{
	//	<key_fields>
	//		<field field_name=\"t_customer0x2ecity\" />
	//	</key_fields>
	aos_assert_r(conf, false);
	AosXmlTagPtr key_confs = conf->getFirstChild("key_fields", false); 
	aos_assert_r(key_confs, false);	

	OmnString fname;
	AosXmlTagPtr k_conf = key_confs->getFirstChild(true);
	while(k_conf)
	{
		fname = k_conf->getAttrStr("field_name", "");
		aos_assert_r(fname != "", false);
		
		mKeyFields.push_back(fname);
		
		k_conf = key_confs->getNextChild();
	}
	
	OmnTagFuncInfo << endl;
	return true;
}

bool	
AosStatistic::configShuffleFields(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	AosXmlTagPtr shuffle_fields_confs = conf->getFirstChild("shuffle_fields", false); 
	if(!shuffle_fields_confs)
		return true;
	OmnString fname;
	AosXmlTagPtr s_conf = shuffle_fields_confs->getFirstChild("field");
	while(s_conf)
	{
		fname = s_conf->getAttrStr("field_name", "");
		aos_assert_r(fname != "", false);
		
		mShuffleFields.push_back(fname);
		
		s_conf = shuffle_fields_confs->getNextChild();
	}
	
	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatistic::configVt2dInfos(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	//<vector2ds>
	//	<vt2d zky_name="vt2d_1" time_unit="_day" time_field_name="xxx">
	//		<measures>
	//			<measure zky_name="sum0x28key_field10x29"/>
	//			<measure zky_name="count0x28key_field10x29"/>
	//		</measures>
	//	</vt2d>
	//	...
	//</vector2ds>
	
	AosXmlTagPtr vt2d_info_confs = conf->getFirstChild("vector2ds", false); 
	aos_assert_r(vt2d_info_confs, false);
	
	bool rslt;
	AosXmlTagPtr each_conf = vt2d_info_confs->getFirstChild("vt2d", true);
	AosXmlTagPtr vt2d_info_conf;
	while(each_conf)
	{
		//rslt = configEachVt2dInfo(rdata, info, each_conf);
		vt2d_info_conf = each_conf->getFirstChild("vt2d_info");
		aos_assert_r(vt2d_info_conf, false);

		AosVt2dInfo info;	
		rslt = info.config(rdata, vt2d_info_conf);
		aos_assert_r(rslt, false);

		mVt2dInfos.push_back(info);
		for(u32 i=0; i<info.mMeasures.size(); i++)
		{
			mMeasureInfos.push_back(info.mMeasures[i]);
		}
		
		each_conf = vt2d_info_confs->getNextChild("vt2d");
	}

	OmnTagFuncInfo << endl;
	return true;
}

/*
bool
AosStatistic::getVt2dIdxByMeasureName(const OmnString &name, vector<int> &vt2d_idxs)
{
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(mVt2dInfos[i].mMeasures[j].mName != name)	continue;
			vt2d_idxs.push_back(i);
		}
	}
	
	//return -1;
	return true;
}
*/

int
AosStatistic::getVt2dIdxByMeasureName(
		const OmnString &name,
		const AosStatTimeUnit::E grpby_time_unit)
{
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		if(mVt2dInfos[i].mGrpbyTimeUnit != grpby_time_unit)	continue; 

		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(mVt2dInfos[i].mMeasures[j].mName == name) return i;
		}
	}
	
	return -1;
}

//yang
int
AosStatistic::getVt2dIdxByMeasureIdx(
		const int idx,
		const AosStatTimeUnit::E grpby_time_unit)
{
	u32 measure_idx = 0;
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		if(mVt2dInfos[i].mGrpbyTimeUnit != grpby_time_unit)	continue;

		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(measure_idx ==(u32) idx) return i;
			measure_idx++;
		}
	}

	return -1;
}


int
AosStatistic::getVt2dIdxByMeasureName(
		const OmnString &name)
{
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(mVt2dInfos[i].mMeasures[j].mName == name) return i;
		}
	}

	return -1;
}

//yang
int
AosStatistic::getVt2dIdxByMeasureIdx(
		const int idx)
{
	int measure_idx = 0;
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(measure_idx == idx) return i;
			measure_idx++;
		}
	}
	return -1;
}


//yang
u32
AosStatistic::getMeasureSize()
{
	u32 measure_size = 0;
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			measure_size++;
		}
	}

	return measure_size;
}

bool 
AosStatistic::getMeasureFields(vector<OmnString> &measureFields)
{
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			//arvin 2015.08.17
			//JIMODB-449
			OmnString measure = mVt2dInfos[i].mMeasures[j].mName;
			measureFields.push_back(measure);
		}
	}
	return true;
}

int
AosStatistic::pickVt2dIdx(const OmnString &name, AosStatTimeUnit::E grpby_time_unit)
{
	int picked_vt2d_idx = -1;
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		for(u32 j=0; j<mVt2dInfos[i].mMeasures.size(); j++)
		{
			if(mVt2dInfos[i].mMeasures[j].mName != name)	continue;
			if(!AosStatTimeUnit::isCompatible(mVt2dInfos[i].mGrpbyTimeUnit,
					grpby_time_unit))	continue;
			
			// this vt2d match.
			if(picked_vt2d_idx == -1 || AosStatTimeUnit::isCompatible(
						mVt2dInfos[picked_vt2d_idx].mGrpbyTimeUnit,
						mVt2dInfos[i].mGrpbyTimeUnit))
			{
				picked_vt2d_idx = i;
			}
		}
	}
	
	return picked_vt2d_idx;
}


bool
AosStatistic::getVt2dInfo(const u32 vt2d_idx, AosVt2dInfo &info)
{
	aos_assert_r(vt2d_idx < mVt2dInfos.size(), false);
	
	info = mVt2dInfos[vt2d_idx];
	return true;
}


bool
AosStatistic::getMeasureInfo(
		const u32 vt2d_idx,
		const u32 vt2d_measure_idx,
		MeasureInfo &m_info)
{
	aos_assert_r(vt2d_idx < mVt2dInfos.size(), false);
	aos_assert_r(vt2d_measure_idx < mVt2dInfos[vt2d_idx].mMeasures.size(), false);

	m_info = mVt2dInfos[vt2d_idx].mMeasures[vt2d_measure_idx];
	return true;
}	
	

bool
AosStatistic::isMeasureRawField(const OmnString &fname)
{
	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		if(mMeasureInfos[i].mFieldName == fname) return true;
	}
	return false;
}


OmnString
AosStatistic::findKeyFieldByMeasureField(const OmnString &measure_field)
{
	for(u32 idx = 0; idx<mKeyFields.size(); idx++)
	{
		if(mKeyFields[idx].findSubString(measure_field, 0) != -1) return mKeyFields[idx];
	}

	return "";
}

bool
AosStatistic::isContainKeyField(const OmnString &field)
{
	for(u32 i=0; i<mKeyFields.size(); i++)
	{
		if(mKeyFields[i] == field)    return true;
	}
	return false;
}


bool
AosStatistic::isContainTimeUnit(const AosStatTimeUnit::E grpby_time_unit)
{
	aos_assert_r(mVt2dInfos.size(), false);

	if(mTimeField == "")
	{
		if(grpby_time_unit == AosStatTimeUnit::eAllTime) return true;
		return false;
	}
	
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		if(AosStatTimeUnit::isCompatible(mVt2dInfos[i].mGrpbyTimeUnit,
					grpby_time_unit))	return true;
	}
	return false;
}


bool
AosStatistic::isContainMeasure(
		const OmnString &measure_name,
		const OmnString &field_name,
		const OmnString &aggr_func_str)
{
	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		if (isContainMeasure(measure_name)) return true;
			
		if(aggr_func_str == "count" && mMeasureInfos[i].mAggrFuncStr == "count")
		{
			if(field_name == "*" || field_name == "0x2a")	return true;
			if(isContainKeyField(field_name))	return true;
		}
	}
	return false;
}


bool
AosStatistic::isContainJoinCond(const OmnString &cond_str)
{
	OmnNotImplementedYet;
	return false;
}
	
bool
AosStatistic::isContainTimeField(
		const OmnString &fname, 
		const AosStatTimeUnit::E grpby_time_unit)
{
	if(!isContainTimeField(fname))	return false;
	
	return isContainTimeUnit(grpby_time_unit);
}

bool
AosStatistic::isContainMeasure(const OmnString &measure_name)
{
	OmnString meaName = measure_name;
	if(meaName.hasPrefix("accu_"))
	{
		meaName = meaName.substr(5,0);
		meaName.replace("_rec_count","0x2a",true);
	}

	//in there, wo assume that measure should be folowing format 
	// E.g max0x28Date_of_birth0x29
	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		//int pos = meaName.indexOf("0x28", 0);
		//aos_assert_r(pos > 0, false);
		//OmnString subStr = meaName.subString(0, pos);
		//meaName.replace(0, pos, subStr.toLower());
		if(mMeasureInfos[i].mName == meaName) return true;
	}

	return false;
}


bool
AosStatistic::hasDistCountMeasure()
{
	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		if(mMeasureInfos[i].mAggrFuncStr == "dist_count")	return true;
	}
	return false;
}
	

AosDataType::E
AosStatistic::getMeasureDataType(const OmnString &name)
{
	OmnString meaName = name;
	if(meaName.hasPrefix("accu_")) 
		meaName = meaName.substr(5,0);

	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		if(mMeasureInfos[i].mName == meaName)
		{
			return mMeasureInfos[i].mDataType;
		}
	}

	return AosDataType::eInvalid;
}
	
AosDataType::E
AosStatistic::getMeasureType(const OmnString &name)
{
	for(u32 i=0; i<mMeasureInfos.size(); i++)
	{
		if(mMeasureInfos[i].mName == name)
		{
			return mMeasureInfos[i].mType;
		}
	}

	return AosDataType::eInvalid;
}

