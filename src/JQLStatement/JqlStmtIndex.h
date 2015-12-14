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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtIndex_H
#define AOS_JQLStatement_JqlStmtIndex_H


#include "JQLStatement/JqlWhere.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlQueryWhereConds.h"
#include "JQLStatement/JqlQueryWhereCond.h"
#include "JQLStatement/JqlLimit.h"
#include "JQLStatement/Ptrs.h"
#include "Util/String.h"

class AosJqlStmtIndex : public AosJqlStatement
{
public:
	struct AosFieldIdxCols
	{
		AosExprList					*mFieldList;
		OmnString					mIndexName;
		OmnString					mType;

		AosFieldIdxCols()
		{
			mFieldList = 0;
		}

		~AosFieldIdxCols() {}
	};


public:
	vector<AosJqlStmtIndex::AosFieldIdxCols*>		mFieldIndexColumns;
	OmnString						mTableName;
	OmnString						mIndexName;
	OmnString						mErrmsg;
	AosExprList						*mKeysExpr;
	vector<AosExprList*>			*mRecords;
	AosJqlLimit						*mLimit;
	AosJqlWhere						*mWhereConds;
	vector<AosJqlQueryWhereCondPtr>	mConds;
	AosJqlQueryWhereCondPtr			mCond;
	bool							mDefaultIndex;
	char 							mSep;

public:
	AosJqlStmtIndex(const OmnString &errmsg);
	AosJqlStmtIndex();
	~AosJqlStmtIndex();

	void setErrmsg(const OmnString &errmsg) {mErrmsg = errmsg;}
	void setTableName(OmnString name);
	void setIndexName(OmnString name);
	void setIsDefault(){mDefaultIndex = true;}
	// void setFieldIndexColumns(vector<AosJqlStmtIndex::AosFieldIdxCols*> *fieldIdxCols);
	void setFieldList(AosJqlStmtIndex::AosFieldIdxCols* field_list);
	void setLimit(AosJqlLimit *limit);
	void setKeysExpr(AosExprList *keys_list);
	void setWhereConds(AosJqlWhere *where_conds);

	// records
	void setIndexRecords(vector<AosExprList*> *records);

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	bool describeIndex(const AosRundataPtr &rdata);
	bool createIndex(const AosRundataPtr &rdata);
	bool showIndexes(const AosRundataPtr &rdata);
	bool dropIndex(const AosRundataPtr &rdata);
	bool listIndex(const AosRundataPtr &rdata);
	bool listShuffleIndex(const AosRundataPtr &rdata);
	int convertToXml(const AosRundataPtr &rdata);
	bool createIndexByRecord(const AosRundataPtr &rdata);
	AosXmlTagPtr createTerm(const AosRundataPtr &rdata);
	OmnString generateQuerFilterConf(AosJqlQueryWhereCondPtr cond);
	virtual AosJqlStatement *clone();
	virtual void dump();
	OmnString getObjName();

	bool isExistIndexName(const AosRundataPtr &rdata);
};

#endif
