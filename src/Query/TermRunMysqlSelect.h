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
// 2013/08/12  Andy Zhang     
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermRunMysqlSelect_h
#define Aos_Query_TermRunMysqlSelect_h

#include "Query/TermIILType.h"
#include "Util/Opr.h"
#include "SEUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataReceiverObj.h"
#include "XmlUtil/Ptrs.h"
#include "Database/Ptrs.h"
#include <vector>


class AosTermRunMysqlSelect : public AosTermIILType
{
private:
	u32                     mDataBasePort;
	i64                     mNumOfField;
	i64                     mCrtIdx;
	i64 					mNumValues;
	OmnString               mUser;
	OmnString               mPasswd;
	OmnString               mIp;
	OmnString               mDatabaseName;
	OmnString               mSqlPath;
	vector<OmnString>       mHeads;
	OmnString				mSql;
	OmnDbTablePtr			mRaw;
	vector<OmnDbRecordPtr>  mRecords;

public:
	AosTermRunMysqlSelect(const bool regflag);
	AosTermRunMysqlSelect(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermRunMysqlSelect();

	virtual bool 	nextDocid(
					const AosQueryTermObjPtr &parent_term,
					u64 &docid, 
					bool &finished, 
					const AosRundataPtr &rdata);
	virtual bool	checkDocid(const u64 &docid, const AosRundataPtr &rdata);

	virtual bool 	getDocidsFromIIL(
					const AosQueryRsltObjPtr &query_rslt,
					const AosBitmapObjPtr &query_bitmap, 
					const AosQueryContextObjPtr &query_context, 
					const AosRundataPtr &rdata);

	virtual i64		getTotal(const AosRundataPtr &rdata);

	virtual void	reset(const AosRundataPtr &rdata);

	virtual bool 	moveTo(
					const i64 &pos, 
					const AosRundataPtr &rdata);

	virtual void	toString(OmnString &str);

	virtual AosXmlTagPtr    getDoc(
					const u64 &docid,
					const AosRundataPtr &rdata);

	virtual AosQueryTermObjPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

	bool loadData(const AosRundataPtr &rdata);


private:
	bool 			parse(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);

};
#endif

