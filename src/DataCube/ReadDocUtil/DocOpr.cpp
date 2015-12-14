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
#include "DataCube/ReadDocUtil/DocOpr.h"

#include "API/AosApi.h"
#include "DataCube/ReadDocUtil/StatDocOpr.h"
#include "DataCube/ReadDocUtil/MultiFixedDocOpr.h"
#include "DataCube/ReadDocUtil/FixedDocOpr.h"
#include "DataCube/ReadDocUtil/NormDocOpr.h"
#include "DataCube/ReadDocUtil/CSVDocOpr.h"
#include "DataCube/ReadDocUtil/CommonDocOpr.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosDocOprPtr
AosDocOpr::getDocOpr(const AosRundataPtr &rdata, const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, 0);
	OmnString tp_str = conf->getAttrStr("doc_type", "");
	AosDocOprPtr doc_opr;
	if(tp_str == "stat_doc")
	{
		doc_opr = OmnNew AosStatDocOpr(); 
	}
	else if(tp_str == "multi_fixed_doc")
	{
		doc_opr = OmnNew AosMultiFixedDocOpr();
	}
	else if(tp_str == "fixed_doc")
	{
		//doc_opr = OmnNew AosFixedDocOpr();
		doc_opr = OmnNew AosCommonDocOpr();
	}
	else if(tp_str == "norm_doc")
	{
		doc_opr = OmnNew AosNormDocOpr();
	}
	else if(tp_str == "csv_doc")
	{
		//doc_opr = OmnNew AosCSVDocOpr();
		doc_opr = OmnNew AosCommonDocOpr();
	}
	else if(tp_str == "common_doc")
	{
		doc_opr = OmnNew AosCommonDocOpr();
	}
	//else if(tp_str == "vt2d_doc")
	//{
	//	doc_opr = OmnNew AosCSVDocOpr();
	//}
	aos_assert_r(doc_opr, 0);

	bool rslt = doc_opr->config(rdata, conf);
	aos_assert_r(rslt, 0);
	return doc_opr;
}
	
bool
AosDocOpr::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	return true;
}


void
AosDocOpr::include()
{
	//OmnNew AosStatDocOpr();
}


u64
AosDocOpr::getEachGroupDocidNum(const u64 total_num)
{
	int pnum = AosGetNumPhysicals();
	int vnum = AosGetNumCubes();
	aos_assert_r(pnum > 0 && vnum > 0, eEachGroupDocidNum);
	return eEachGroupDocidNum * vnum / pnum;
}


// Ken Lee, 2015/01/06
AosBuffPtr
AosDocOpr::getAllResp(
		const AosBuffPtr metaResp,
		IdVector &totalDocids,
		IdVector &schemaids,
		map<u64, AosBuffPtr> &allResps)
{
	AosBuffPtr big_resp = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	if (metaResp) big_resp->setBuff(metaResp);

	u64 docid = 0;
	u64 schemaid = 0;
	map<u64, AosBuffPtr>::iterator itr;
	AosBuffPtr docid_resp;
	for (u32 i=0; i<totalDocids.size(); i++)
	{
		docid = totalDocids[i];
		
		itr = allResps.find(docid);
		if (itr == allResps.end()) continue;

		aos_assert_r(docid == itr->first, 0);
		docid_resp = itr->second;
		schemaid = *(u64 *)(docid_resp->data() + sizeof(u32));
		//big_resp->setU64(docid);
		big_resp->setBuff(docid_resp);
		schemaids.push_back(schemaid);
	}
	
	return big_resp;
}

