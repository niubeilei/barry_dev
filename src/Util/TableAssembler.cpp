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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/TableAssembler.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Util/StrU64Array.h"
#include "Util/CompareFun.h"


AosTableAssembler::AosTableAssembler(
		const AosXmlTagPtr &conf, 
		AosRundata *rdata)
:
mNumPhysicals(AosGetNumPhysicals())
{
	if (mNumPhysicals <= 0)
	{
		OmnThrowException("invalid physicals");
		return;
	}

	// Create the trans client
	OmnNotImplementedYet;
	
	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosTableAssembler::AosTableAssembler(
		const AosXmlTagPtr &conf, 
		const AosTransClientPtr &transclt, 
		AosRundata *rdata)
:
mTransClient(transclt),
mNumPhysicals(AosGetNumPhysicals())
{
	if (mNumPhysicals <= 0)
	{
		OmnThrowException("invalid physicals");
		return;
	}

	bool rslt = config(conf, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosTableAssembler::~AosTableAssembler()
{
}


bool
AosTableAssembler::config(const AosXmlTagPtr &conf, AosRundata *rdata)
{
	aos_assert_rr(conf, rdata, false);
	mConfig = conf->clone(AosMemoryCheckerArgsBegin);

	bool stable = conf->getAttrBool("stable_sort", false);
	AosXmlTagPtr funcfg = conf->getFirstChild("Function");
	aos_assert_rr(funcfg, rdata, false);
	AosCompareFun1Ptr cmpfun;// = AosCompareFunc::getCompareFunc(funcfg);
	aos_assert_rr(cmpfun, rdata, false);
	for (int i=0; i<mNumPhysicals; i++)
	{
		mTables.push_back(OmnNew AosStrU64Array(cmpfun, stable));
	}
	return true;
}


bool 
AosTableAssembler::addValue(
		const char* data, 
		const u32 len, 
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosTableAssembler::addValue(
		const char* data, 
		const u32 len, 
		int  phyid, 
		AosRundata *rdata)
{
	// 1. It checks whether 'data' has a docid. If not, it needs to assign one to it.
	// 2. Use its docid to determine to which bucket 'data' belongs. 
	// 3. When a bucket is full, send the contents.
	aos_assert_rr(data, rdata, false);
	//if (distid == 0)
	//{
		//docid = nextDocid(rdata);
		//aos_assert_rr(docid, rdata, false);
		//*(u64*)&data[mPosition] = docid;
	//}
	//int phyid = docid % mNumVirtuals % mNumPhysicals;
	aos_assert_r(phyid >= 0 && phyid<mNumPhysicals, false);
	bool rslt = mTables[phyid]->addValue(data, len, rdata);
	if (mTables[phyid]->dataLength() > eMaxTableSize)
	{
		sendTable(phyid, rdata);
	}
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosTableAssembler::sendTable(int phyid,  AosRundata *rdata)
{
	mTables[phyid]->sort();
	AosXmlTagPtr xml = mConfig->clone(AosMemoryCheckerArgsBegin);
	xml->addNode1("Data");
	AosXmlTagPtr datanode = xml->getFirstChild("Data");
	aos_assert_r(datanode, false);
	datanode->setTextBinary(mTables[phyid]->getBuff());
	bool rslt = mTransClient->addTrans(rdata, xml, false, phyid);
	mTables[phyid]->clear();
	return rslt;
}


bool 
AosTableAssembler::finish(AosRundata *rdata)
{
	for (size_t i=0; i<mTables.size(); i++)
	{
		bool rslt = sendTable(i, rdata);
		aos_assert_r(rslt, false);
		//sendFinish(i, rdata);
		aos_assert_r(rslt, false);
	}	
	return true;
}


/*
bool 
AosTableAssembler::sendFinish(int phyid,  AosRundata *rdata)
{
	//AosTagXmlPtr
	//bool rslt = mTransClient->addTrans(rdata, xml, false, phyid);
}
*/
#endif
