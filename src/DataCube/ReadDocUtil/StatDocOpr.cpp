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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/ReadDocUtil/StatDocOpr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
//#include "SEInterfaces/StatCltObj.h"
#include "StatTrans/BatchGetStatDocsTrans.h"

AosStatDocOpr::AosStatDocOpr()
{
}


AosStatDocOpr::~AosStatDocOpr()
{
}


bool
AosStatDocOpr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	//<stat_qry>
	//	<qry_measures>
	//		<measure field_name="key_field7" agr_type="sum" zky_name="sum0x28key_field70x29"/>
	//	</qry_measures>
	//	<group_by_keys>
	//		<field field_name="f21"/>
	//	</group_by_keys>
	//	<group_by_time time_unit="_day"/>
	//	<times/>
	//</stat_qry>
	aos_assert_r(conf, false);

	mStatQryConf = conf->getFirstChild("stat_qry");
	aos_assert_r(mStatQryConf, false);
	
	AosXmlTagPtr stat_cubes_conf = conf->getFirstChild("stat_cubes");
	aos_assert_r(stat_cubes_conf, false);

	AosXmlTagPtr each_conf = stat_cubes_conf->getFirstChild();
	u32 cube_id;
	while(each_conf)
	{
		cube_id = each_conf->getAttrU32("cube_id", 0);	

		mStatCubeConfMap.insert(make_pair(cube_id, each_conf));
		each_conf = stat_cubes_conf->getNextChild();
	}
	
	return true;
}


bool
AosStatDocOpr::shufferDocids(
		const AosRundataPtr &rdata,
		const AosBuffPtr &docids_buff,
		const u32 docid_num,
		map<u32, IdVector> &docid_grp)
{
	u64 stat_docid;
	u32 remain = docid_num;
	while(remain--)
	{
		stat_docid = docids_buff->getU64(0);
		aos_assert_r(stat_docid, false);
		
		pushDocidToDocidGrp(rdata, stat_docid, docid_grp);
	}
	
	return true;
}

bool
AosStatDocOpr::shufferDocids(
		const AosRundataPtr &rdata,
		IdVector &docids, 
		map<u32, IdVector> &docid_grp)
{
	u64 stat_docid;
	for(u32 i=0; i<docids.size(); i++)
	{
		stat_docid = docids[i];
		aos_assert_r(stat_docid, false);
		
		pushDocidToDocidGrp(rdata, stat_docid, docid_grp);
	}
	return true;
}


bool
AosStatDocOpr::pushDocidToDocidGrp(
		const AosRundataPtr &rdata,
		const u64 stat_docid,
		map<u32, IdVector> &docid_grp)
{
	u32 cube_id = stat_docid >> 32; 

	map<u32, IdVector>::iterator itr = docid_grp.find(cube_id);
	if(itr == docid_grp.end())
	{
		IdVector id_v;
		pair<map<u32, IdVector>::iterator, bool> itr_rslt;
		itr_rslt = docid_grp.insert(make_pair(cube_id, id_v));
		aos_assert_r(itr_rslt.second, false);
		itr = itr_rslt.first;
	}
	
	aos_assert_r(itr != docid_grp.end(), false);
	//(itr->second).push_back(stat_docid);
	u64 sub_sdocid = (u32)stat_docid;
	(itr->second).push_back(sub_sdocid);
	return true;
}

	
bool
AosStatDocOpr::sendReadDocTrans(
		const AosRundataPtr &rdata,
		const u64 reqid,
		const u32 cube_id,
		IdVector &stat_docids,
		const AosAsyncRespCallerPtr &resp_caller)
{
	aos_assert_r(resp_caller, false);
	
	map<u32, AosXmlTagPtr>::iterator itr = mStatCubeConfMap.find(cube_id);
	aos_assert_r(itr != mStatCubeConfMap.end(), false);
	AosXmlTagPtr cube_conf = itr->second;
	aos_assert_r(cube_conf, false);

	AosTransPtr trans = OmnNew AosBatchGetStatDocsTrans(cube_id,
			cube_conf, stat_docids, mStatQryConf, 
			reqid, resp_caller);
	AosSendTransAsyncResp(rdata, trans);
	
	return true;
}


u64
AosStatDocOpr::getEachGroupDocidNum(const u64 total_num)
{
	return total_num;
}


bool
AosStatDocOpr::getNextDocidResp(
		const AosBuffPtr &big_buff,
		bool &finished,
		u64 &docid,
		AosBuffPtr &docid_resp)
{
	// big_buff format.
	// 0.	resp_len				u32
	// 1.	docid					u64
	// 2.	key_str_len				int
	// 3.	key_str					key_str_len
	// 4.	time_cond_value_len		int
	// 5.	time_cond_value			time_cond_value_len
	// 6.	.... (repeat 4 and 5)
	aos_assert_r(big_buff, false);
	if(big_buff->getCrtIdx() >= big_buff->dataLen())
	{
		finished = true;
		return true;
	}

	finished = false;

	docid = big_buff->getU64(0);	// Ketty temp.
	aos_assert_r(docid != 0, false);
	
	u32 remain_len = big_buff->dataLen() - big_buff->getCrtIdx();
	docid_resp = big_buff->getBuff(remain_len, true AosMemoryCheckerArgs);
	return true;
}


AosBuffPtr
AosStatDocOpr::getMetaResp(const AosBuffPtr &big_buff)
{
	return 0;
}


// Ken Lee, 2015/01/06
AosBuffPtr
AosStatDocOpr::getAllResp(
		const AosBuffPtr metaResp,
		IdVector &totalDocids,
		map<u64, AosBuffPtr> &allResps)
{
	AosBuffPtr big_resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	if (metaResp) big_resp->setBuff(metaResp);

	u64 docid = 0;
	map<u64, AosBuffPtr>::iterator itr;
	AosBuffPtr docid_resp;
	for (u32 i=0; i<totalDocids.size(); i++)
	{
		docid = totalDocids[i];
		
		itr = allResps.find(docid);
		if (itr == allResps.end()) continue;

		aos_assert_r(docid == itr->first, 0);
		docid_resp = itr->second;
		big_resp->setBuff(docid_resp);
	}
	
	return big_resp;
}



/*
bool
AosStatDocOpr::getNextDocidResp(
		const AosBuffPtr &big_buff,
		bool &finished,
		u64 &docid,
		AosBuffPtr &docid_resp)
{
	// big_buff format.
	// 0.	resp_len				u32
	// 1.	docid					u64
	// 2.	key_str_len				int
	// 3.	key_str					key_str_len
	// 4.	time_cond_value_len		int
	// 5.	time_cond_value			time_cond_value_len
	// 6.	.... (repeat 4 and 5)
	aos_assert_r(big_buff, false);
	if(big_buff->getCrtIdx() >= big_buff->dataLen())
	{
		finished = true;
		return true;
	}

	finished = false;
	
	u32 crt_idx = big_buff->getCrtIdx();
	u32 resp_len = big_buff->getU32(0);
	aos_assert_r(resp_len, false);
	
	docid = big_buff->getU64(0);
	aos_assert_r(docid != 0, false);
	
	big_buff->setCrtIdx(crt_idx);
	docid_resp = big_buff->getBuff(sizeof(u32) + resp_len, true AosMemoryCheckerArgs);
	
	return true;
}

AosBuffPtr
AosStatDocOpr::getMetaResp(const AosBuffPtr &big_buff)
{
	//u64 total_max_stat_num = big_buff->getU64(0);
	
	//AosBuffPtr meta_resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	//meta_resp->setU64(total_max_stat_num);
	//return meta_resp;
	OmnNotImplementedYet;
	return 0;
}

*/
