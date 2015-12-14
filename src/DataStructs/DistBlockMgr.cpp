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
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/DistBlockMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"

AosDistBlockMgr::AosDistBlockMgr()
{
}


AosDistBlockMgr::~AosDistBlockMgr()
{
}


bool 
AosDistBlockMgr::getStatDoc(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &docid,
		const bool create,
		char *&stat_doc,
		int64_t &stat_doc_len)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDistBlockMgr::saveDistBlock(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block)
{
	OmnShouldNeverComeHere;
	return false;
}

//bool 
//AosDistBlockMgr::modifyStatDoc(
//		const AosRundataPtr &rdata,
//		AosDistBlock &dist_block,
//		const u64 &ext_docid,
//		const AosBuffPtr &stat_doc)
//{
//	OmnShouldNeverComeHere;
//	return false;
//}

/*
bool 
AosDistBlockMgr::getStatDoc(
		const AosRundataPtr &rdata, 
		AosDistBlock &dist_block,
		const u64 &stat_docid,
		char *&stat_doc,
		int64_t &stat_doc_len)
{
	OmnShouldNeverComeHere;
	return false;
}

u64  
AosDistBlockMgr::getDocidTester(
		const u64 &stat_docid,
		const u64 &statdocsize)
{
	OmnShouldNeverComeHere;
	return 0;
}
*/


u64 
AosDistBlockMgr::getDocPerDistBlock()
{
	OmnShouldNeverComeHere;
	return 0;
}


//void 
//AosDistBlockMgr::resetContents(const AosBuffPtr &buff)
//{
//	OmnShouldNeverComeHere;
//}


u64 
AosDistBlockMgr::getEntryByStatId(const u64 &stat_id)
{
	OmnShouldNeverComeHere;
	return 0;
}


void 
AosDistBlockMgr::setSnapShots(
			map<int, u64> &snap_ids, 
			map<int, u64> &binary_snap_ids)
{
	OmnShouldNeverComeHere;
}


AosXmlTagPtr 
AosDistBlockMgr::createBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosXmlTagPtr 
AosDistBlockMgr::retrieveBinaryDoc(
			const AosRundataPtr &rdata,
			const u64 &docid, 
			AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosDistBlockMgr::modifyBinaryDoc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc,
			const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return 0;
}

	
bool
AosDistBlockMgr::resetStatIdMgr(const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


u64
AosDistBlockMgr::getDocsPerDistBlock()
{
	OmnShouldNeverComeHere;
	return 0;
}


