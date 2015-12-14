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
#include "DataCube/Jimos/DataCubeReadDocBySort.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "DataCube/ReadDocUtil/DocOpr.h"
#include "DataCube/ReadDocUtil/ReadDocBySortUnit.h"
#include "Debug/Debug.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeReadDocBySort_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeReadDocBySort(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosDataCubeReadDocBySort::AosDataCubeReadDocBySort(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_READDOC_BYSORT, version),
mCrtReadIdx(0)
{
}


AosDataCubeReadDocBySort::~AosDataCubeReadDocBySort()
{
}


AosDataConnectorObjPtr 
AosDataCubeReadDocBySort::cloneDataConnector()
{
	return OmnNew AosDataCubeReadDocBySort(*this);
}


AosJimoPtr 
AosDataCubeReadDocBySort::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeReadDocBySort(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool
AosDataCubeReadDocBySort::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	//  worker_doc format 
	//  <data_cube jimo_objid="datacube_stat" >
	//		<docids num="xxx" ><![BDATA[xxxx]]>	
	//		</docids>	
	//		<conds>
	//		</conds>
	//	</data_cube>
	
	aos_assert_rr(worker_doc, rdata, false);
 	OmnTagFuncInfo << ": datacube xml is: " << 
		worker_doc->toString() << endl;
	
	mOpr = AosDocOpr::getDocOpr(rdata, worker_doc);
	aos_assert_r(mOpr, false);
	
	AosXmlTagPtr qry_conds_conf = worker_doc->getFirstChild("conds");
	if(qry_conds_conf)
	{	
		OmnTagFuncInfo << ": conds xml is: " << qry_conds_conf->toString() << endl;
		worker_doc->setAttr("start_idx", "0");
		//worker_doc->setAttr("psize", "900000");
		worker_doc->setAttr("psize", "10000000");
		mQueryReq = AosQueryReqObj::createQueryStaticNew(worker_doc, rdata);
		aos_assert_r(mQueryReq, false);
	}

	// temp.
	//vector<u64> docids;
	//bool rslt = getDocidsByConf(worker_doc, docids);
	//aos_assert_r(rslt && docids.size(), false);
	
	//rslt = splitDocidsBySize(docids);
	//aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDataCubeReadDocBySort::start(AosRundata *rdata)
{
	OmnTagFuncInfo << " DataCube by Sort start() " << endl;
	if(!mQueryReq)	return true;	
	
	// read docid from query.
	bool rslt = mQueryReq->runQuery(rdata);
	aos_assert_r(rslt, false);
	
	AosQueryRsltObjPtr query_rslt;
	rslt = mQueryReq->generateRslt(query_rslt, rdata);
	aos_assert_r(rslt && query_rslt, false);

	if (query_rslt->getNumDocs() <= 0) return true;

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	u64 docid = 0;
	
	bool finish = false;
	while (!finish)
	{
		rslt = query_rslt->nextDocid(docid, finish, rdata);
		aos_assert_r(rslt, false);

		if (finish) break;
		buff->setU64(docid);
	}

	buff->reset();
	rslt = setValueBuff(buff, rdata);
	aos_assert_r(rslt, false);
OmnScreen << "AosDataCubeReadDocBySort AosDataCubeReadDocBySort num_docs:" << query_rslt->getNumDocs() << ";"<< endl;	
	return true;
}

	
bool
AosDataCubeReadDocBySort::setValueBuff(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	aos_assert_r(buff, false);
	
	vector<u64> all_docids;
	while(buff->getCrtIdx() < buff->dataLen())
	{
		u64 docid = buff->getU64(0);
		if (docid != 0) all_docids.push_back(docid);
	}

	//for(u32 i=1; i<=100; i++)
	//{
	//	all_docids.push_back(i);
	//}

	OmnScreen << "Total Docid size:" << all_docids.size() << endl;
	aos_assert_r(all_docids.size() && mOpr, false);
	bool rslt = splitDocidsBySize(rdata, all_docids);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataCubeReadDocBySort::splitDocidsBySize(AosRundata *rdata, vector<u64> &docids)
{
	u64 each_grp_docidnum = mOpr->getEachGroupDocidNum(docids.size());
	
	vector<u64> id_vt;
	for(u32 i=0; i<docids.size(); i++)
	{
		u64 docid = docids[i];
		id_vt.push_back(docid);
		if(id_vt.size() < each_grp_docidnum)	continue;
		
		addReadUnit(rdata, mOpr, id_vt);
		id_vt.clear();
	}

	if(id_vt.size())
	{
		addReadUnit(rdata, mOpr, id_vt);
	}

	return true;
}


bool
AosDataCubeReadDocBySort::addReadUnit(
		AosRundata *rdata,
		AosDocOprPtr &opr,
		vector<u64> &docids)
{
	AosDataConnectorCallerObjPtr thisptr(this, false);
	AosReadDocBySortUnitPtr reader = OmnNew AosReadDocBySortUnit(rdata, opr, docids);
	reader->setCaller(thisptr);
	mDocReaderGrp.push_back(reader);
	return true;	
}


bool
AosDataCubeReadDocBySort::getDocidsByConf(
		const AosXmlTagPtr &conf,
		vector<u64> &docids)
{
	AosXmlTagPtr docids_xml = conf->getFirstChild("docids");
	aos_assert_r(docids_xml, false);

	u32 total = docids_xml->getAttrU32("num", 0);
	aos_assert_r(total, false);

	AosBuffPtr ids_buff = docids_xml->getNodeTextBinaryUnCopy("id_buff" AosMemoryCheckerArgs);
	aos_assert_r(ids_buff, false);
	
	u64 docid;
	for(u32 i=0; i<total; i++)
	{
		docid = ids_buff->getU64(0);
		aos_assert_r(docid, false);
		docids.push_back(docid);
	}
	return true;
}

bool
AosDataCubeReadDocBySort::readData(const u64 reqid, AosRundata *rdata)
{
	if (isReadFinish())
	{
		if (!mCaller)
		{
			OmnAlarm << "already finished" << enderr;
			return true;
		}	
		mCaller->callBack(reqid, 0, true);
		return true;
	}

	bool rslt = mDocReaderGrp[mCrtReadIdx]->readData(reqid, rdata);
	aos_assert_r(rslt, false);
	mCrtReadIdx++;
	return true;
}

bool
AosDataCubeReadDocBySort::isReadFinish()
{
	return mCrtReadIdx >= mDocReaderGrp.size();
}

void
AosDataCubeReadDocBySort::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		bool finish)
{
	aos_assert(finish);
	aos_assert(mCaller);
	mCaller->callBack(reqid, buff, isReadFinish());
	if (isReadFinish())
	{
		//mCaller = 0;
		mDocReaderGrp.clear();
	}
}


void
AosDataCubeReadDocBySort::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;
}

