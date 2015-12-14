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
// This class implements a two-dimensional array:
// 	big_array[element_id][value]
// Elements are identified by integers, starting from 0. Elements are
// always appended. The array can grow by appending elements. There 
// is no logical limitations on the size of a big array (i.e., the 
// maximum number of elements a big array may contain), but it is 
// possible to add the maximum limitation on it.
//
// Each element may contain an array of values. Different elements may
// contain different number of values (in a sense, the sizes of elements
// are variable). Values are addressed by indexes.
//
// Example:
// 		value = big_array[1234][567]
//
// One may define the minnimum and maximum value indexes. For instance, 
// if the values are epoch days, and the first day ever possible for the
// big array is 12345. The minimum and maximum are used for error checking 
// only.
//
// Modification History:
// 2013/12/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Vector2D.h"

#include "API/AosApi.h"
#include "DataStructs/RemoteDistBlockMgr.h"
#include "DataStructs/LocalDistBlockMgr.h"
#include "DataStructs/StatBatchReader.h"
#include "DataStructs/DataStructsUtil.h"
#include "StatUtil/AggregationFunc.h"
#include "Util/DataTypes.h"

AosVector2D::AosVector2D(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &stat_vt2d_conf)
:
mRootStatDocSize(0),
mExtensionStatDocSize(0),
mStartTimeSlot(0),
mDistBlockMgr(0),
mCtrlDocObjid(""),
mCtrlDocid(0),
mProc(0),
mAgrFunc(0),
mBatchReader(0),
mInvalidStatValue(NULL),
mValueSize(0)
{
	config(rdata, stat_vt2d_conf);
}


AosVector2D::AosVector2D(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &stat_vt2d_conf,
		const AosXmlTagPtr &stat_measure_conf)
:
mRootStatDocSize(0),
mExtensionStatDocSize(0),
mStartTimeSlot(0),
mDistBlockMgr(0),
mCtrlDocObjid(""),
mCtrlDocid(0),
mProc(0),
mAgrFunc(0),
mBatchReader(0),
mInvalidStatValue(NULL),
mValueSize(0)
{
	configRetrieveAndCubSvr(rdata, stat_vt2d_conf, stat_measure_conf);
}


AosVector2D::AosVector2D()
{
}


AosVector2D::~AosVector2D()
{
	if (mInvalidStatValue != NULL)
	{
		delete mInvalidStatValue; 	// Ketty 2014/05/23
	}
}


void
AosVector2D::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_vt2d_conf)
{
	//stat_vt2d_conf:
	//<stat_vt2d_conf>
	//	<vector2d_control root_control_objid ="stat_root_example1" extension_control_objid="stat_ext_example1" zky_data_type="int64"/>
	//	<vector2d root_stat_doc_size="11500" index_size="3000" time_bucket_weight="1000" start_time_slot="14610" extension_doc_size="8000"/>
	//	<agr_type type="sum"/>
	//</stat_vt2d_conf>
	
	aos_assert(stat_vt2d_conf);
	bool rslt = configStructProcPriv(rdata, stat_vt2d_conf);
	aos_assert(rslt);

	AosXmlTagPtr def = stat_vt2d_conf->getFirstChild("vector2d");
	aos_assert(def);

	AosXmlTagPtr vt2d_conf = stat_vt2d_conf->getFirstChild("vector2d_control");
	aos_assert(vt2d_conf);

	int64_t index_size;
	u64 time_bucket_weight;
	rslt = configBasePriv(def, vt2d_conf, index_size, time_bucket_weight);
	aos_assert(rslt);

	mCtrlDocObjid = vt2d_conf->getAttrStr("root_control_objid", "");
	//u64 docid = AosGetDocidByObjid(mCtrlDocObjid, rdata);
	mCtrlDocid = AosGetDocidByObjid(mCtrlDocObjid, rdata);
	aos_assert(mCtrlDocid);
	mDistBlockMgr = configDistBlockMgr(rdata, mCtrlDocid, 0, false);
	aos_assert(mDistBlockMgr);

	aos_assert(mProc);
	AosRootStatDoc::Parameter parm(
			index_size, 
			mExtensionStatDocSize, 
			time_bucket_weight,
			mDataType,
			mProc->getDftValue());
	mParm = parm;
	
	mValueSize = AosDataType::getValueSize(mDataType);
}


AosDistBlockMgrPtr
AosVector2D::configDistBlockMgr(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const u64 &bsnap_id,
		const bool need_snap_shot)
{
	AosBuffPtr buff;
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->retrieveBinaryDocByStat(
			docid, buff, 0, bsnap_id, rdata);
	aos_assert_r(doc, 0);

	// maybe no buff. Ketty 2014/07/21
	//aos_assert_r(buff && buff->dataLen() > 0, 0);
	AosDistBlockMgrPtr dbmgr = OmnNew AosRemoteDistBlockMgr(doc, buff, need_snap_shot);
	aos_assert_r(dbmgr, 0);
	return dbmgr; 
}


void
AosVector2D::configRetrieveAndCubSvr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_vt2d_conf, 
		const AosXmlTagPtr &stat_measure_conf)
{
	// stat_measure_conf format:
	//	<measure time_unit="_day">
	//		<measure_field field_name="sales"/>
	//		<agr_type type="sum"/>
	//		<vector2d_control root_control_objid ="xxx" extension_control_objid="xxx" zky_data_type="int64"/>
	//	</measure>
	//
	// stat_vt2d_conf format:
	//	<vector2d root_stat_doc_size="xxx" index_size="xxx" time_bucket_weight="xxx" start_time_slot="xxx" extension_doc_size="xxx"/>

	aos_assert(stat_measure_conf);
	bool rslt = configStructProcPriv(rdata, stat_measure_conf);
	aos_assert(rslt);

	AosXmlTagPtr vt2d_conf = stat_measure_conf->getFirstChild("vector2d_control");
	aos_assert(vt2d_conf);

	int64_t index_size;
	u64 time_bucket_weight;
	rslt = configBasePriv(stat_vt2d_conf, vt2d_conf, index_size, time_bucket_weight);
	aos_assert(rslt);

	mDistBlockMgr = 0;

	aos_assert(mProc);

	AosRootStatDoc::Parameter parm(
			index_size, 
			mExtensionStatDocSize, 
			time_bucket_weight,
			mDataType,
			mProc->getDftValue());
	mParm = parm;
	mValueSize = AosDataType::getValueSize(mDataType);

	mBatchReader = OmnNew AosStatBatchReader();

	AosXmlTagPtr agr_type_conf = stat_measure_conf->getFirstChild("agr_type");
	aos_assert(agr_type_conf);
	AosAggregationType::E agr_tp = AosAggregationType::toEnum(agr_type_conf->getAttrStr("type", ""));
	aos_assert(AosAggregationType::isValid(agr_tp));
	mAgrFunc = AosAggregationFunc::getAggregation(agr_tp);
	aos_assert(mAgrFunc);
	mAgrFuncRaw = mAgrFunc.getPtr();

	OmnString time_unit_str = stat_measure_conf->getAttrStr("time_unit", "");
	aos_assert(time_unit_str != "");
	mTimeUnit = AosStatTimeUnit::getTimeUnit(time_unit_str); 
	
	mInvalidStatValue = OmnNew char[mValueSize + 10];	// Ketty 2014/05/23
	//memset(mInvalidStatValue, 0, stat_vv_len + 10);
	memset(mInvalidStatValue, mParm.getDftValue(), mValueSize + 10);
	
	mQueryMeasureConf = stat_measure_conf;
}


bool
AosVector2D::configBasePriv(
		const AosXmlTagPtr &vt2d_ctrl_conf,
		const AosXmlTagPtr &vt2d_conf,
		int64_t &index_size,
		u64 &time_bucket_weight)
{
	aos_assert_r(vt2d_ctrl_conf, false);
	mRootStatDocSize = vt2d_ctrl_conf->getAttrInt("root_stat_doc_size", -1);
	mExtensionStatDocSize = vt2d_ctrl_conf->getAttrInt("extension_doc_size", -1);

	index_size = vt2d_ctrl_conf->getAttrInt("index_size", -1);
	aos_assert_r(index_size + mExtensionStatDocSize < mRootStatDocSize, false);

	time_bucket_weight = vt2d_ctrl_conf->getAttrU64("time_bucket_weight", 1);
	mStartTimeSlot = vt2d_ctrl_conf->getAttrU64("start_time_slot", 0);

	aos_assert_r(vt2d_conf, false);
	mDataType = AosDataType::toEnum(vt2d_conf->getAttrStr(AOSTAG_DATA_TYPE, ""));
	aos_assert_r(AosDataType::isValid(mDataType), false);
	return true;
}


bool
AosVector2D::configStructProcPriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_vt2d_conf)
{
	AosXmlTagPtr agr_type_conf = stat_vt2d_conf->getFirstChild("agr_type");
	aos_assert_r(agr_type_conf, false);
	AosXmlTagPtr jimo_doc = AosDataStructsUtil::pickStructProcJimoDoc(rdata, agr_type_conf);
	aos_assert_r(jimo_doc, false);

	mProc = AosStructProcObj::createStructProcStatic(rdata.getPtr(), jimo_doc);
	aos_assert_r(mProc, false);
	return true;
}


void
AosVector2D::initSnapShot(
		map<int, u64> &snap_ids, 
		map<int, u64> &bsnap_ids)
{
	aos_assert(mDistBlockMgr);
	mDistBlockMgr->setSnapShots(snap_ids, bsnap_ids);
}


void
AosVector2D::initDistBlock(
		const AosRundataPtr &rdata,
		map<int, u64> &root_ctrl_doc_snap_ids)
{
	u64 bsnap_id = 0;
	u64 docid = AosGetDocidByObjid(mCtrlDocObjid, rdata);	
	if (root_ctrl_doc_snap_ids.size() != 0)
	{
		int virtual_id = AosGetCubeId(docid);
		map<int, u64>::iterator itr = root_ctrl_doc_snap_ids.find(virtual_id);	
		aos_assert(itr != root_ctrl_doc_snap_ids.end());
		bsnap_id = itr->second;
		aos_assert(bsnap_id);
	}

	aos_assert(!mDistBlockMgr);
	mDistBlockMgr = configDistBlockMgr(
			rdata, docid, bsnap_id, true);
	aos_assert(mDistBlockMgr);
}

void
AosVector2D::calculateRootStatDocSize()
{
	//Root Stat Doc Size Selection
	//Selecting the right size for the root stat doc is important. 
	//Root stat docs should be big enough to store the indexes and the values for one time bucket. 
	//As an example, if the statistics is collected daily, the time bucket is 100 days, 
	//and the statistics should support 30 years, below are the calculations:
	//  index 
	//	Total days: 365*30 = 10,950
	//	Total index entries: 110 (10950 / 100 = 110)
	//	Index Entry Size: 8(u64) + 8(u64) = 16
	//	Num Index Size: 8(u64) 
	//	Total Index Size = 1,768 (110 * 16 + 8)
	//
	//	body bucket
	//	Value size: 4(int)
	//	Size for one time bucket: 400 (100days * 4)
	//
	//	other
	//	KeyLen Size: 4(u32)
	//	Key Size: 300(AosIILUtil::eMaxStrValueLen) 
	//	mGeneral Size: 8(u64)
	//	other Total Size: 312
	//
	//	total
	//	Root stat doc size: 2,480 (1,768 + 400 + 312) 
	//	(other_total_size  + value_bucket_size + total_index_size)
	//	ext stat doc size: 400  (value_bucket_size)
	//
	// 	root_doc_per_distblock  =  AosDistBlock::eDfmDistBlockSize/Root_stat_doc_size;
	//  ext_doc_per_distblock = AosDistBlock::eDfmDistBlockSize/ext_stat_doc_size;


	//If the time bucket size is 365, the calculations are:
	//	Total days: 365*30=10,950
	//	Total index entries: 30
	//	Index Entry Size: 8(u64) + 8(u64) = 16
	//	Total Index Size = 480
	//	Value size: 4(int)
	//	Size for one time bucket: 400
	//	KeyLen Size: 4(u32)
	//	Key Size: 300
	//	mTotal Size: 8(u64)
	//	Root stat doc size: 1192(480 + 400 + 300 + 8 + 4)
	//	Root stat doc sizes affect statistics updates. When new inputs are collected and processed, 
	//	the statistics need to be updated. If the current time bucket is kept in the root stat docs, 
	//	this means it may need to read all the stat docs for the statistics. The larger the root stat doc size is,
	//	the more bytes it needs to read whenever their values are updated. 
}


