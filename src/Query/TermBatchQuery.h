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
// 04/16/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Query_TermBatchQuery_h
#define Aos_Query_TermBatchQuery_h

#include "Query/QueryTerm.h"
#include "Query/TermIILType.h"
#include "UtilTime/TimeGran.h"
#include "UtilTime/TimeInfo.h"


class AosTermBatchQuery : public AosTermIILType
{
	enum
	{
		eMaxSize = 10000000
	};

	struct FieldDef
	{
		AosDataType::E	data_type;
		OmnString		aname;
	};

private:
	OmnString				mQueryObjid;
	i64						mStartIdx;
	i64						mEndIdx;
	i64						mSize;
	i64						mNumValues;
    bool                	mDataLoaded;
	i64						mCrtIdx;
	i64						mDftValue;
	bool					mNoMoreDocs;
	AosBuffPtr				mBuff;
	vector<AosQrUtil::FieldDef>		mFieldDef;


public:
	AosTermBatchQuery(const bool regflag);
	AosTermBatchQuery(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTermBatchQuery();

	virtual bool nextDocid(
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
	virtual bool 	moveTo(const i64 &pos, const AosRundataPtr &rdata);
	virtual void	toString(OmnString &str);
	virtual AosXmlTagPtr    getDoc(const u64 &docid, const AosRundataPtr &rdata);
	virtual AosQueryTermObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	bool loadData(const AosRundataPtr &rdata);


private:
	bool parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	AosXmlTagPtr createRecord(const i64 &index, const AosRundataPtr &rdata);
	bool getOneFieldValue(OmnString &value, const AosRundataPtr &rdata);

	virtual bool	setPagesize(const i64 &psize){mSize = psize; return true;}
	virtual bool	setStartIdx(const i64 &num){mStartIdx = num; return true;}

};
#endif

