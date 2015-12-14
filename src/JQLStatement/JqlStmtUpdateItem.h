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
#ifndef AOS_JQLStatement_JqlStmtUpdateItem_H
#define AOS_JQLStatement_JqlStmtUpdateItem_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "JQLStatement/JQLCommon.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosJqlStmtUpdateItem : public AosJqlStatement
{
private:
	AosJqlStmtUpdate*				mPUpdate;				
	AosExprList*					mPExprList;
	AosExprObj*						mPTable;
	AosExpr*						mOptWhere;
	OmnString 						mTableName;
	map<OmnString, OmnString>		mValues;

	OmnString						mErrmsg;


public:
	AosJqlStmtUpdateItem();
	AosJqlStmtUpdateItem(AosJqlStmtUpdate *stmt);
	AosJqlStmtUpdateItem(const OmnString &errmsg);
	~AosJqlStmtUpdateItem();

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setErrmsg(const OmnString &errmsg) {mErrmsg = errmsg;}
	bool modifyDoc(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &doc);

	static bool updateData( 
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &doc,
						map<OmnString, OmnString> *fieldValueMap);

};

#endif
