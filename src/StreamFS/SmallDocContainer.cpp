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
// 2013/10/25 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "StreamFS/SmallDocContainer.h"
#include "API/AosApi.h"
#include "XmlUtil/DocTypes.h"
#include "XmlUtil/XmlDoc.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgrUtil/DevLocation.h"
#include "SEInterfaces/DocSvrObj.h"


bool 
AosSmallDocContainer::addDocs(
		const u64 &docid,
		const AosBuffPtr &contents,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid > 0, false);
	aos_assert_r(contents && rdata, false);
	bool rslt = AosDocSvrObj::getDocSvr()->addSmallDocs(docid, contents, trans_id, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosSmallDocContainer::retrieveDoc(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosDataRecordObjPtr &doc)
{
	// Young 2014/11/04
	OmnNotImplementedYet;	
	return true;
}


bool 
AosSmallDocContainer::ageOutDocs(
		const AosRundataPtr &rdata,
		const u64 group_id,
		const AosTransId &trans_id)
{
	// delete doc with docid.
	bool rslt = AosDocSvrObj::getDocSvr()->deleteBinaryDoc(group_id, rdata, trans_id, 0);
	aos_assert_r(rslt, false);
	return true;
}


AosBuffPtr
AosSmallDocContainer::retrieveDoc(
		const AosRundataPtr &rdata,
		const u64 &smallDocid, 
		const u32 &entryNums)
{
	aos_assert_r(entryNums > 0, NULL);

	u64 group_id = getRealDocid(smallDocid, entryNums);
	AosBuffPtr logs_buff = AosDocSvrObj::getDocSvr()->getSmallDocs(rdata, group_id);
	aos_assert_r(logs_buff, 0);

	// 2. get doc with index 
	u32 entryIdx = getSmallDocIdx(smallDocid, entryNums);
	for (u32 i=0; i < entryIdx; i++)
	{
		u32 crtEntryLen = logs_buff->getU32(0);	
		aos_assert_r(crtEntryLen > 0, 0);
		logs_buff->incIndex(crtEntryLen);
	}
	u32 entryLen = logs_buff->getU32(0);

	// Young, 2013/11/07
	// buff = metadata + log
	AosBuffPtr buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);	
	buff = logs_buff->getBuff(entryLen, true AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);

	return buff;
}


u64 		
AosSmallDocContainer::getRealDocid(
		const u64 smallDocid, 
		const u32 entryNums)
{
	// the samll docid is not real docid
	AosDocType::E type = AosDocType::getDocType(smallDocid);
	u64 ownSmallDocid = AosXmlDoc::getOwnDocid(smallDocid);
	u64 rdocid = ownSmallDocid / entryNums;
	AosDocType::setDocidType(type, rdocid);

	return rdocid;
}


u32 		
AosSmallDocContainer::getSmallDocIdx(
		const u64 smallDocid, 
		const u32 entryNums)
{
	u64 ownSmallDocid = AosXmlDoc::getOwnDocid(smallDocid);
	
	return ownSmallDocid % entryNums;
}
