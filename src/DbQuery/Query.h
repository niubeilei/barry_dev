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
// 07/28/2011	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DbQuery_Query_h
#define AOS_DbQuery_Query_h

#include "ValueSel/ValueSel.h"
#include "Query/Ptrs.h"
#include "QueryClient/Ptrs.h"
#include "QueryUtil/QueryCondInfo.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/IpAddr.h"
#include "Util/Opr.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class OmnFile;
OmnDefineSingletonClass(AosQuerySingleton,
						AosQuery,
						AosQuerySelf,
						OmnSingletonObjId::eQuery,
						"Query");

class AosQuery : virtual public AosQueryObj,
				 virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eFieldTypeAttr = 1,
		eFieldTypeSubtag = 2
	};

	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	u32				mTransId;
	bool			mIsLocal;
	OmnTcpClientPtr	mConn;

public:
	AosQuery();
	~AosQuery();

    // Singleton class interface
    static AosQuery*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    //virtual OmnString   getSysObjName() const {return "AosQuery";}
    virtual bool 		config(const AosXmlTagPtr &def);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
    
	bool      	start(const AosXmlTagPtr &config);
	
	static AosQueryReqObjPtr parseQuery(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);

	static bool doQuery(
			const AosQueryReqObjPtr &query,
			const AosXmlTagPtr &query_xml,
			const AosRundataPtr &rdata);

	bool runQuery(
		 	const AosXmlTagPtr &query,
		 	AosXmlTagPtr &results,
		 	const AosRundataPtr &rdata);

	bool runQuery(
		 	const OmnString &query,
		 	AosXmlTagPtr &results,
		 	const AosRundataPtr &rdata);

	// Chen Ding, 2013/02/07
	bool runQuery(
			const i64 &startidx,
			const i64 &psize,
			const OmnString &iilname, 
			const AosValueRslt &value1,
			const AosValueRslt &value2,
			const AosOpr opr, 
			AosQueryRsltObjPtr &query_rslt,
			const bool reverse,
			const bool with_docid,
			const AosRundataPtr &rdata);

	bool runQuery(
			const i64 &startidx,
			const i64 &psize,
			const OmnString &iilname, 
			const u64 &value, 
			const AosOpr opr, 
			const u64 *docids, 
			const AosRundataPtr &rdata);

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
			const AosRundataPtr &rdata);

	bool runQuery(
			const OmnString &ctnr_objid, 
			const OmnString &aname,
			const OmnString &value, 
			const AosOpr opr,
			const bool reverse,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &bitmap,
			const AosRundataPtr &rdata);

	bool runQuery(
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
			const AosRundataPtr &rdata);

	bool runQuery(
			const i64 &startidx,
			const i64 &psize,
			const vector<AosQueryCondInfo> &conds,
			AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	u64 getMember(
			const OmnString &container_objid, 
			const OmnString &id_name, 
			const OmnString &id_value,
			bool &is_unique,
			const AosRundataPtr &rdata);

	virtual i64 retrieveContainerMembers(
			const i64 &startidx,
			const i64 &psize,
			const OmnString &ctnr_objid,
			const OmnString &anames, 
			OmnString &results,
			const AosRundataPtr &rdata);

	virtual i64 retrieveContainerMembers(
			const OmnString &ctnr_objid,
			vector<OmnString> &objids, 
			const i64 &psize,
			const i64 &startidx,
			const bool reverse,
			const AosRundataPtr &rdata);
	
	virtual i64 retrieveContainerDocidsMembers(
			const OmnString &ctnr_objid,
			vector<u64> &docids, 
			const i64 &psize,
			const i64 &startidx,
			const bool reverse,
			const AosRundataPtr &rdata);

	bool runQuery(
			const i64 &startidx,
			const i64 &psize,
			const u64 *iilid,
			const AosValueRslt *value,
			const AosOpr *opr, 
			const bool *reverse,
			const bool *order,
			const i64 &num_conds,
			AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool getAllContainers(
			const i64 &startidx,
			const i64 &psize,
			const bool reverse,
			AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata);

	bool getUserDomains1(
			const i64 &startidx, 
			const i64 &psize, 
			const bool reverse, 
			AosQueryRsltObjPtr &values, 
			const AosRundataPtr &rdata);

	bool runQuery(
			const i64 &startidx,
			const i64 &psize,
			const OmnString &ctnr_objid, 
			const OmnString &value, 
			const AosOpr opr, 
			const bool reverse, 
			OmnString *values,
			u64 *docids, 
			const bool with_values,
			const AosRundataPtr &rdata);

	// Chen Ding, 2013/05/08
	bool runQuery(
		 	const AosRundataPtr &rdata,
			const i64 &startidx, 
			const i64 &psize,
		 	const AosXmlTagPtr &query,
			AosQueryRsltObjPtr &query_rslt);

private:
	inline int getFieldType(OmnString &name)
	{
		// This function determines the field type:
		// 	1. If it is in the form:
		// 			tagname/_#text
		// 	   it removes '/_#text' and returns eSubtag.
		// 	2. Otherwise, it returns eFieldTypeAttr.
		int len = name.length();
		if (len < 8)
		{
			return eFieldTypeAttr;
		}
		const char *data = name.data();

		if (data[len-1] == 't' &&
			data[len-2] == 'x' &&
			data[len-3] == 'e' &&
			data[len-4] == 't' &&
			data[len-5] == '#' &&
			data[len-6] == '_' &&
			data[len-7] == '/')
		{
			name.setLength(len-7);
			return eFieldTypeSubtag;
		}
		return eFieldTypeAttr;
	}
};
#endif
