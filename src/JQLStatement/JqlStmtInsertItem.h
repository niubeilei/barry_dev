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
#ifndef AOS_JQLStatement_JqlStmtInsertItem_H
#define AOS_JQLStatement_JqlStmtInsertItem_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "JQLStatement/JQLCommon.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlStmtInsertItem : public AosJqlStatement
{
private:
	AosJqlStmtInsert*				mPInsert;
	AosExprList*					mPNames;
	AosExprList*					mPValues;
	OmnString						mTableName;
	OmnString						mErrmsg;

public:
	AosJqlStmtInsertItem(const OmnString &errmsg);
	AosJqlStmtInsertItem(AosJqlStmtInsert *stmt);
	AosJqlStmtInsertItem(const AosJqlStmtInsertItem &rhs);
	~AosJqlStmtInsertItem();

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setErrmsg(const OmnString &errmsg) {mErrmsg = errmsg;}
	bool modifyDoc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc);

	bool runQuery(
			const AosRundataPtr &rdata,
			const OmnString &iilname,
			const AosQueryRsltObjPtr &query_rslt,
			const AosBitmapObjPtr &bitmap,
			const AosQueryContextObjPtr &query_context);

	static AosXmlTagPtr insertData( 
						const AosRundataPtr &rdata,
						const OmnString dbName,
						const OmnString tableName,
						map<OmnString, OmnString> *fieldValueMap);
							
	static AosXmlTagPtr insertJobStat(
						const AosRundataPtr &rdata,
						map<OmnString, OmnString> *fieldValueMap);

	static AosXmlTagPtr insertJobData(
						const AosRundataPtr &rdata,
						map<OmnString, OmnString> *fieldValueMap);

	static AosXmlTagPtr insertJobDataStat(
						const AosRundataPtr &rdata,
						map<OmnString, OmnString> *fieldValueMap);


};

#endif
