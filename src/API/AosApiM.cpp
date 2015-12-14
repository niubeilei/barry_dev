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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 06/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiM.h"

#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/IndexMgrObj.h"

bool AosModifyDocAttrStr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc, 
		const OmnString &aname, 
		const OmnString &value, 
		const bool value_unique, 
		const bool docid_unique)
{
	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
	aos_assert_r(theobj, 0);
	return theobj->modifyAttrStr(doc, aname, value,
			value_unique, docid_unique, true, rdata);
}


bool AosModifyDocAttrStr(
		const AosRundataPtr &rdata,
		const u64 &docid, 
		const OmnString &aname, 
		const OmnString &value, 
		const bool value_unique, 
		const bool docid_unique)
{
	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
	aos_assert_r(theobj, 0);

	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
	aos_assert_r(doc, false);

	return theobj->modifyAttrStr(doc, aname, value,
			value_unique, docid_unique, true, rdata);
}


bool AosModifyDocAttrU64(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc, 
		const OmnString &aname, 
		const u64 value, 
		const bool value_unique, 
		const bool docid_unique)
{
	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
	aos_assert_r(theobj, 0);
	return theobj->modifyAttrU64(doc, aname, value,
			value_unique, docid_unique, true, rdata);
}


/*
bool AosModifyDocAttrStr(
		const AosXmlTagPtr &doc, 
		const OmnString &aname, 
		const OmnString &value, 
		const bool value_unique,
		const bool docid_unique,
		const bool checkSecurity,
		AosRundata *rdata)
{
	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
	aos_assert_r(theobj, 0);
	return theobj->modifyAttr(doc, aname, value,
			false, false, checkSecurity, rdata);
}
*/


bool AosModifyDoc(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
	aos_assert_r(theobj, 0);
	return theobj->modifyObj(doc, rdata);
}


int AosMaxFilesPerDir()
{
	// Chen Ding, 11/07/2012
	return 15000;
}


bool AosModifyDocAttrByWorker(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker)
{
	bool rslt = AosActionObj::runAction(rdata, AOSACTTYPE_CREATEDOC, worker);
	aos_assert_r(rslt, 0);
	return rdata->getCreatedDoc();
}


//bool AosModifyBinaryDoc(
//		const AosXmlTagPtr &doc,
//		const AosBuffPtr &buff, 
//		AosRundata *rdata)
//{
//	AosDocClientObjPtr theobj = AosDocClientObj::getDocClient();
//	aos_assert_r(theobj, 0);
//	return theobj->modifyBinaryDoc(doc, buff, rdata);
//}


bool AosModifyIndexEntry(
		AosRundata *rdata,
		const AosXmlTagPtr &def)
{
	return AosIndexMgrObj::modifyEntryStatic(rdata, def);
}

