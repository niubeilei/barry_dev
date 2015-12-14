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
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiD_h
#define AOS_API_ApiD_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Opr.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosValueRslt;

extern bool AosDoesDocExist(const u64 &docid, const AosRundataPtr &rdata);
extern bool AosDoesDocExist(const OmnString &objid, const AosRundataPtr &rdata);

extern bool AosDbQuery(
					const u64 &iilid, 
					const AosQueryRsltObjPtr &query_rslt, 
					const AosBitmapObjPtr &bitmap,
					const AosOpr opr,
					const OmnString &value, 
					const AosRundataPtr &rdata);

extern bool AosDbSetU64ValueDocUnique(
					u64 &iilid, 
					const bool createFlag,
					const u64 &key,
					const u64 &docid, 
					const bool must_same,
					AosRundata *rdata);

extern bool AosDbSetU64ValueDocUniqueToTable(
					u64 &iilid, 
					const bool createFlag,
					const u64 &key,
					const u64 &docid, 
					const bool must_same,
					AosRundata *rdata);

extern bool AosDbSetStrValueDocUniqueToTable(
					u64 &iilid, 
					const bool createFlag,
					const OmnString &key,
					const u64 &docid, 
					const bool must_same,
					const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
					const AosXmlTagPtr &query,
					AosXmlTagPtr &results,
					const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
					const OmnString &query,
					AosXmlTagPtr &results,
					const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
		const OmnString &iilname, 
		const u64 &value, 
		const AosOpr opr, 
		const u64 *docids, 
		int &psize, 
		const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
		const OmnString &iilname, 
		const OmnString &aname,
		const OmnString &value,
		const AosOpr opr, 
		const bool reverse,
		OmnString *values, 
		u64 *docids, 
		int &psize, 
		const bool with_values,
		const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
		const OmnString &ctnr_objid, 
		const OmnString &aname,
		const OmnString &value, 
		const AosOpr opr,
		const bool reverse,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &bitmap,
		const AosRundataPtr &rdata);

extern bool AosDbRunQuery(
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
		const AosRundataPtr &rdata);

extern bool AosDeleteIIL(
		const u64 &iilid,
		const bool true_delete,
		const AosRundataPtr &rdata);

extern bool AosDeleteIIL(
		const OmnString &iilname,
		const bool true_delete,
		const AosRundataPtr &rdata);

extern bool AosDeleteBinaryDoc(
		const OmnString &objid, 
		AosRundata *rdata);

extern bool AosDeleteDocByDocid(
		const u64 &docid, 
		const AosRundataPtr &rdata);

extern bool AosDeleteDocByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata);

extern bool AosDeleteBinaryDoc(
		const u64 &docid, 
		AosRundata *rdata);

// extern bool DQ(AosRundata *rdata, 
// 		const OmnString &query, 
// 		const AosDataPickerObjPtr &caller);

#endif
