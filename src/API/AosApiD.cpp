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
// 03/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiD.h"

#include "Rundata/Rundata.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Containers.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/QueryObj.h"
#include "XmlUtil/XmlTag.h"

bool AosDoesDocExist(const u64 &docid, const AosRundataPtr &rdata)
{
	// In the current implementations, it retrieves the doc. In the future,
	// it should be optimized by checking whether the doc exists, not actually
	// retrieving the doc.
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	AosXmlTagPtr doc = docclient->getDocByDocid(docid, rdata);
	if (doc) return true;
	return false;
}

bool AosDoesDocExist(const OmnString &objid, const AosRundataPtr &rdata)
{
	// In the current implementations, it retrieves the doc. In the future,
	// it should be optimized by checking whether the doc exists, not actually
	// retrieving the doc.
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	AosXmlTagPtr doc = docclient->getDocByObjid(objid, rdata);
	if (doc) return true;
	return false;
}

bool AosDbQuery(
		const u64 &iilid, 
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosOpr opr,
		const OmnString &value, 
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);

	// Chen Ding, 08/29/2012
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(opr);
	query_context->setStrValue(value);
	return iilclient->querySafe(iilid, query_rslt, bitmap, query_context, rdata);
}


bool AosDbSetU64ValueDocUnique(
		u64 &iilid, 
		const bool createFlag,
		const u64 &key,
		const u64 &docid, 
		const bool must_same,
		AosRundata *rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->setU64ValueDocUnique(iilid, createFlag, key, docid, must_same, rdata);
}


bool AosDbSetU64ValueDocUniqueToTable(
		u64 &iilid, 
		const bool createFlag,
		const u64 &key,
		const u64 &docid, 
		const bool must_same,
		AosRundata *rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->setU64ValueDocUniqueToTable(
			iilid, createFlag, key, docid, must_same, rdata);
}


bool AosDbSetStrValueDocUniqueToTable(
		u64 &iilid, 
		const bool createFlag,
		const OmnString &key,
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->setStrValueDocUniqueToTable(
			iilid, createFlag, key, docid, must_same, rdata);
}


bool AosDbRunQuery(
		const AosXmlTagPtr &query,
		AosXmlTagPtr &results,
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(query, results, rdata);
}


bool AosDbRunQuery(
		const OmnString &query,
		AosXmlTagPtr &results,
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(query, results, rdata);
}


bool AosDbRunQuery(
		const int startidx,					// Chen Ding, 05/16/2012
		const int psize,					// Chen Ding, 05/16/2012
		const OmnString &iilname, 
		const u64 &value, 
		const AosOpr opr, 
		const u64 *docids, 
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(startidx, psize, iilname, value, opr, docids, rdata);
}


bool AosDbRunQuery(
		const int startidx,					// Chen Ding, 05/16/2012
		const int psize,					// Chen Ding, 05/16/2012
		const OmnString &iilname, 
		const OmnString &aname,
		const OmnString &value,
		const AosOpr opr, 
		const bool reverse,
		OmnString *values, 
		u64 *docids, 
		const bool with_values,
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(startidx, psize, iilname, aname, value, opr, reverse, 
			values, docids, with_values, rdata);
}


bool AosDbRunQuery(
		const OmnString &ctnr_objid, 
		const OmnString &aname,
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &bitmap,
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(ctnr_objid, aname, value, opr, reverse, 
			query_rslt, bitmap, rdata);
}


bool AosDbRunQuery(
		const int start_idx,
		const int psize,
		const OmnString *ctnr_objids, 
		const OmnString *anames, 
		const AosValueRslt *value, 
		const AosOpr *opr, 
		const bool *reverse,
		const bool *order,
		const int num_conds,
		AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	AosQueryObjPtr queryobj = AosQueryObj::getQueryObj();
	aos_assert_rr(queryobj, rdata, false);
	return queryobj->runQuery(start_idx, psize, 
			ctnr_objids, anames, value, opr, reverse, order, num_conds, 
			query_rslt, rdata);
}

bool AosDeleteIIL(
		const u64 &iilid,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient =  AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->deleteIIL(iilid, true_delete, rdata);
}


bool AosDeleteIIL(
		const OmnString &iilname,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient =  AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->deleteIIL(iilname, true_delete, rdata);
}


bool AosDeleteBinaryDoc(
		const OmnString &objid, 
		AosRundata *rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);

	return docclient->deleteBinaryDoc(objid, rdata);
}


bool AosDeleteDocByDocid(
		const u64	&docid, 
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	OmnString str;
	str << docid;

	return docclient->deleteObj(rdata, str, "", "", false);
}


bool AosDeleteDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	return docclient->deleteObj(rdata, "", objid, "", false);
}


bool AosDeleteBinaryDoc(
		const u64 &docid, 
		AosRundata *rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);

	return docclient->deleteBinaryDoc(docid, rdata);
}


/*
bool DQ(AosRundata *rdata, 
		const OmnString &query, 
		const AosDataPickerObjPtr &caller)
{
	AosDataPickerMgrObjPtr object = AosDataPickerMgrObj::getObject();
	aos_assert_rr(object, rdata, false);
	object->run(rdata, query, caller);
}
*/

