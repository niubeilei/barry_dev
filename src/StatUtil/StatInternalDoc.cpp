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
// 2014/04/02	Created by Ketty 
//
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatInternalDoc.h"

#include "XmlUtil/XmlTag.h"
#include "StatTrans/CreateStatMetaFileTrans.h"

AosStatInternalDoc::AosStatInternalDoc(const OmnString &stat_identify_key)
:
mStatIdentifyKey(stat_identify_key)
{
}

AosStatInternalDoc::~AosStatInternalDoc()
{
}


bool
AosStatInternalDoc::config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
{
	if(!conf) return true;

	bool rslt;
	mCrtObjid = conf->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(mCrtObjid != "", false);

	OmnString crt_identify_key = conf->getAttrStr("zky_stat_identify_key", "");
	aos_assert_r(mStatIdentifyKey == crt_identify_key, false);

	mKeyFieldsConf = conf->getFirstChild("key_fields"); 
	aos_assert_r(mKeyFieldsConf, false); 

	mKeyConnConf = conf->getFirstChild("stat_key_conn");
	aos_assert_r(mKeyConnConf, false); 

	// maybe no timeField
	mTimeFieldConf = conf->getFirstChild("time_field"); 
	//aos_assert_r(mTimeFieldConf, false); 

	AosXmlTagPtr vt2d_infos_conf = conf->getFirstChild("vector2ds");	
	aos_assert_r(vt2d_infos_conf, false);

	AosXmlTagPtr vt2d_conf = vt2d_infos_conf->getFirstChild();
	AosXmlTagPtr vt2d_conn_conf, vt2d_info_conf;
	AosVt2dInfo vt2d_info;
	while(vt2d_conf)
	{
		vt2d_info_conf = vt2d_conf->getFirstChild("vt2d_info");
		aos_assert_r(vt2d_info_conf, false);

		rslt = vt2d_info.config(rdata, vt2d_info_conf);
		aos_assert_r(rslt, false);

		vt2d_conn_conf = vt2d_conf->getFirstChild("vt2d_conn");
		aos_assert_r(vt2d_conn_conf, false);
		
		mVt2dInfos.push_back(vt2d_info);
		mVt2dConnConfs.push_back(vt2d_conn_conf);

		vt2d_conf = vt2d_infos_conf->getNextChild();
	}
	
	AosXmlTagPtr stat_cube_confs = conf->getFirstChild("stat_cubes", false);
	aos_assert_r(stat_cube_confs, false);
	AosXmlTagPtr cube_conf = stat_cube_confs->getFirstChild();
	while(cube_conf)
	{
		mStatCubeConfs.push_back(cube_conf);
		cube_conf = stat_cube_confs->getNextChild();	
	}
	aos_assert_r(mStatCubeConfs.size() == (u32)AosGetNumCubes(), false);

	OmnTagFuncInfo << "config xml is: " << conf->toString() << endl; 
	return true;
}


bool
AosStatInternalDoc::addStatDefDoc(
		const AosRundataPtr &rdata,
		AosStatDefineDoc &def_doc)
{
	OmnString time_fname = def_doc.getTimeFieldName();
	AosStatTimeUnit::E grpby_time_unit = def_doc.getGrpbyTimeUnit();
	if(time_fname != "")	aos_assert_r(grpby_time_unit != AosStatTimeUnit::eInvalid, false);

	if(!mKeyFieldsConf)
	{
		mKeyFieldsConf = def_doc.getKeyFieldsConf();	
		mKeyConnConf = def_doc.getKeyConnConf();
		
		if(time_fname != "")
		{
			OmnString time_field_conf = "<time_field ";
			time_field_conf << "field_name=\"" << time_fname << "\" " 
				<< "time_format=\"" << def_doc.getTimeFormat() << "\" "
				<< "></time_field>";
			mTimeFieldConf = AosXmlParser::parse(time_field_conf AosMemoryCheckerArgs);
		}
	}

	mCondConf = def_doc.getCondConf();

	mShuffleFieldsConf = def_doc.getShuffleFieldsConf();

	vector<MeasureInfo>	new_m_infos;
	vector<MeasureInfo> crt_m_infos;
	def_doc.getMeasureInfo(rdata, crt_m_infos);
	for(u32 i=0; i<crt_m_infos.size(); i++)
	{
		if(measureExist(crt_m_infos[i].mName, grpby_time_unit))	continue;	
		new_m_infos.push_back(crt_m_infos[i]);
	}
	if(new_m_infos.size() == 0)	return true;

	AosXmlTagPtr vt2d_conn_conf = getVt2dConnConf(rdata, 
			def_doc, grpby_time_unit, time_fname);
	bool rslt = addNewVt2dInfo(rdata, vt2d_conn_conf, 
			grpby_time_unit, time_fname, new_m_infos);
	aos_assert_r(rslt, false);

	OmnTagFuncInfo << "config xml is: " << vt2d_conn_conf->toString() << endl; 
	return true;
}


AosXmlTagPtr
AosStatInternalDoc::getVt2dConnConf(
		const AosRundataPtr &rdata,
		AosStatDefineDoc &def_doc,
		const AosStatTimeUnit::E time_unit,
		const OmnString &time_fname)
{
	AosXmlTagPtr vt2d_conn_conf = def_doc.getVt2dConnConf();
	
	vt2d_conn_conf = vt2d_conn_conf->clone(AosMemoryCheckerArgsBegin);
	if(time_fname == "")
	{
		// means this statistic has no time_field
		vt2d_conn_conf->setAttr("time_bucket_weight", 1);
		return vt2d_conn_conf;
	}

	switch(time_unit)
	{
	case AosStatTimeUnit::eEpochHour:
		vt2d_conn_conf->setAttr("time_bucket_weight", 30 * 24);
		break;

	case AosStatTimeUnit::eEpochDay:
		vt2d_conn_conf->setAttr("time_bucket_weight", 30);
		break;
	
	case AosStatTimeUnit::eEpochWeek:
		vt2d_conn_conf->setAttr("time_bucket_weight", 4);
		break;

	case AosStatTimeUnit::eEpochMonth:
		vt2d_conn_conf->setAttr("time_bucket_weight", 1);
		break;
	
	case AosStatTimeUnit::eEpochYear:
		vt2d_conn_conf->setAttr("time_bucket_weight", 1);
		break;

	default:
		break;
	}
	
	return vt2d_conn_conf;
}


bool
AosStatInternalDoc::measureExist(
		const OmnString &measure_name,
		AosStatTimeUnit::E time_unit)
{
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		if(mVt2dInfos[i].mGrpbyTimeUnit == time_unit && 
				(mVt2dInfos[i].getMeasureIdx(measure_name) != -1))	return true;
	}

	OmnTagFuncInfo << "v2d info size is: " << mVt2dInfos.size() << endl;
	return false;
}


bool
AosStatInternalDoc::addNewVt2dInfo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &vt2d_conn_conf,
		const AosStatTimeUnit::E grpby_time_unit,
		const OmnString &time_fname,
		vector<MeasureInfo>	&new_m_infos)
{
	bool rslt;
	AosVt2dInfo info;
	info.mVt2dName = "vt2d_";
	info.mVt2dName << mVt2dInfos.size();
	
	info.mGrpbyTimeUnit = grpby_time_unit;
	info.mTimeFieldName = time_fname;
	info.mHasValidFlag = vt2d_conn_conf->getAttrBool("has_valid_flag", true); 
	
	for(u32 i=0; i<new_m_infos.size(); i++)
	{
		info.mMeasures.push_back(new_m_infos[i]);
	}
	
	mVt2dInfos.push_back(info);
	mVt2dConnConfs.push_back(vt2d_conn_conf);

	if(mStatCubeConfs.size() == 0)
	{
		rslt = initStatCubeConfs(rdata);
		aos_assert_r(rslt, false);
	}

	int cube_id;
	OmnString file_prefix;
	u64 meta_fileid;
	AosXmlTagPtr cube_vt2d_conn_conf;
	OmnString vt2d_cube_conf_str;
	AosXmlTagPtr vt2d_cube_conf;
	AosXmlTagPtr vt2ds_conf;
	for(u32 i=0; i<mStatCubeConfs.size(); i++)
	{
		cube_id = mStatCubeConfs[i]->getAttrInt("cube_id", -1);	
		aos_assert_r(cube_id != -1, false);
		
		file_prefix = "vt2d_conn_meta";
		meta_fileid = createStatMetaFile(rdata, cube_id, file_prefix);
		aos_assert_r(meta_fileid, false);
	
		cube_vt2d_conn_conf = vt2d_conn_conf->clone(AosMemoryCheckerArgsBegin);
		cube_vt2d_conn_conf->setAttr("meta_fileid", meta_fileid);	

		vt2d_cube_conf_str = "<vt2d>";
		vt2d_cube_conf_str << cube_vt2d_conn_conf->toString()
				<< info.toXmlString(rdata)
				<< "</vt2d>";
		vt2d_cube_conf = AosXmlParser::parse(vt2d_cube_conf_str AosMemoryCheckerArgs);

		vt2ds_conf = mStatCubeConfs[i]->getFirstChild("vector2ds");
		aos_assert_r(vt2ds_conf, false);
		vt2ds_conf->addNode(vt2d_cube_conf);
	}

	OmnTagFuncInfo << "v2d info size is: " << mStatCubeConfs.size() << endl;
	return true;
}


bool
AosStatInternalDoc::initStatCubeConfs(const AosRundataPtr &rdata)
{
	if(mStatCubeConfs.size())	return true;

	OmnString cube_conf_str;
	OmnString file_prefix;
	u64 meta_fileid;
	AosXmlTagPtr cube_key_conn_conf;
	AosXmlTagPtr cube_conf;

	int num_cubes = AosGetNumCubes();
	aos_assert_r(num_cubes > 0, false);
	for(u32 cube_id=0; cube_id<(u32)num_cubes; cube_id++)
	{
		file_prefix = "stat_keyconn_meta";
		meta_fileid = createStatMetaFile(rdata, cube_id, file_prefix);
		aos_assert_r(meta_fileid, false);
		
		cube_key_conn_conf = mKeyConnConf->clone(AosMemoryCheckerArgsBegin);
		cube_key_conn_conf->setAttr("meta_fileid", meta_fileid);	
		
		cube_conf_str = "<cube cube_id=\"";
		cube_conf_str << cube_id << "\" >"
				<< cube_key_conn_conf->toString()
				<< "<vector2ds>"
				<< "</vector2ds>"
				<< "</cube>";
	
		cube_conf = AosXmlParser::parse(cube_conf_str AosMemoryCheckerArgs);
		mStatCubeConfs.push_back(cube_conf);
	}
	OmnTagFuncInfo << endl;
	return true;
}


OmnString
AosStatInternalDoc::toXmlString(const AosRundataPtr &rdata)
{
	aos_assert_r(mVt2dInfos.size() == mVt2dConnConfs.size(), "");

	OmnString str = "<statistic ";
	str << "zky_stat_identify_key=\"" << mStatIdentifyKey << "\" ";
	if(mCrtObjid != "")
	{
		str << AOSTAG_OBJID << "=\"" << mCrtObjid << "\" ";
	}
	str << ">"
		<< mKeyFieldsConf->toString()
		<< mKeyConnConf->toString();
	if(mTimeFieldConf)
	{
		str << mTimeFieldConf->toString(); 
	}

	if(mCondConf)
	{
		str << mCondConf->toString();
	}

	if(mShuffleFieldsConf)
	{
		str << mShuffleFieldsConf->toString();
	}

	str << "<vector2ds>";
	for(u32 i=0; i<mVt2dInfos.size(); i++)
	{
		str << "<vt2d>"
			<< mVt2dConnConfs[i]->toString()
			<< mVt2dInfos[i].toXmlString(rdata)
			<< "</vt2d>";
	}
		
	str << "</vector2ds>"
		<< "<stat_cubes>";
	for(u32 i=0; i<mStatCubeConfs.size(); i++)
	{
		str << mStatCubeConfs[i]->toString();
	}
	
	str << "</stat_cubes>"
		<< "</statistic>";
	return str;
}


u64
AosStatInternalDoc::createStatMetaFile(
		const AosRundataPtr &rdata,
		const u32 cube_id,
		const OmnString &file_prefix)
{
	bool svr_death;
	AosBuffPtr resp;
	AosTransPtr trans = OmnNew AosCreateStatMetaFileTrans(cube_id, file_prefix);
	AosSendTrans(rdata, trans, resp, svr_death);

	if(svr_death)
	{
		OmnScreen << "create stat meta file error! svr death!" << svr_death;
		return 0;
	}
	
	aos_assert_r(resp, 0);
	u64 file_id = resp->getU64(0);
	return file_id;
}



