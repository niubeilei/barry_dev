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
// 01/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryObj_h
#define Aos_SEInterfaces_QueryObj_h

#include "QueryUtil/QueryCondInfo.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Opr.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

class AosValueRslt;

class AosQueryObj : virtual public OmnRCObject
{
private:
	static AosQueryObjPtr smQueryObj;

public:
	virtual bool runQuery(
		 				const AosXmlTagPtr &query,
		 				AosXmlTagPtr &results,
		 				const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
		 				const OmnString &query,
		 				AosXmlTagPtr &results,
		 				const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
						const i64 &startidx,
						const i64 &psize,
						const OmnString &iilname, 
						const u64 &value, 
						const AosOpr opr, 
						const u64 *docids, 
						const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
						const i64 &startidx,
						const i64 &psize,
						const OmnString &iilname, 
						const OmnString &aname,
						const OmnString &value,
						const AosOpr opr, 
						const bool reverse,
						OmnString *values, 
						u64 *docids, 
						const bool with_values,
						const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
						const OmnString &ctnr_objid, 
						const OmnString &aname,
						const OmnString &value, 
						const AosOpr opr,
						const bool reverse,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &bitmap,
						const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
						const i64 &startidx,
						const i64 &psize,
						const OmnString *ctnr_objids, 
						const OmnString *anames, 
						const AosValueRslt *value, 
						const AosOpr *opr, 
						const bool *reverse,
						const bool *order,
						const i64 &num_conds,
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata) = 0;

	virtual bool runQuery(
						const i64 &startidx,
						const i64 &psize,
						const vector<AosQueryCondInfo> &conds,
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata) = 0;

	virtual u64 getMember(const OmnString &container_objid, 
						const OmnString &id_name, 
						const OmnString &id_value,
						bool &is_unique,
						const AosRundataPtr &rdata) = 0;

	virtual bool getAllContainers(
						const i64 &startidx, 
						const i64 &psize,
						const bool reverse,
						AosQueryRsltObjPtr &query_rslt,
						const AosRundataPtr &rdata) = 0;

	virtual bool getUserDomains1(
						const i64 &startidx, 
						const i64 &psize, 
						const bool reverse, 
						AosQueryRsltObjPtr &values, 
						const AosRundataPtr &rdata) = 0;

	virtual i64 retrieveContainerMembers(
						const i64 &startidx,
						const i64 &psize,
						const OmnString &ctnr_objid,
						const OmnString &anames, 
						OmnString &results,
						const AosRundataPtr &rdata) = 0;

	virtual i64 retrieveContainerMembers(
						const OmnString &ctnr_objid,
						vector<OmnString> &objids, 
						const i64 &psize,
						const i64 &startidx,
						const bool reverse,
						const AosRundataPtr &rdata) = 0;

	// Chen Ding, 2013/02/07
	virtual bool runQuery(
						const i64 &startidx,
						const i64 &psize,
						const OmnString &iilname, 
						const AosValueRslt &value1,
						const AosValueRslt &value2,
						const AosOpr opr, 
						AosQueryRsltObjPtr &query_rslt,
						const bool reverse,
						const bool with_docid,
						const AosRundataPtr &rdata) = 0;

	// Chen Ding, 2013/05/08
	virtual bool runQuery(
		 				const AosRundataPtr &rdata,
						const i64 &startidx, 
						const i64 &psize,
		 				const AosXmlTagPtr &query,
						AosQueryRsltObjPtr &query_rslt) = 0;

	static void setQueryObj(const AosQueryObjPtr &q) {smQueryObj = q;}
	static AosQueryObjPtr getQueryObj() {return smQueryObj;}
};

#endif
