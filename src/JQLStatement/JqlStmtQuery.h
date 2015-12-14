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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtQuery_H
#define AOS_JQLStatement_JqlStmtQuery_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlSelect.h"
#include "Dataset/Dataset.h"

#include "Util/String.h"
#include <vector>
#include <map>


class AosJqlSelectField;
class AosJqlStmtQuery : public AosJqlStatement
{
private:
	AosJqlSelectPtr				mSelectPtr;				
	AosJqlSelectFieldList*   	mExprs;
	OmnString 					mErrmsg;
	AosRundataPtr				mRundata;

	OmnString 					mQueryStr;

	//add query result dataset for subquery
	AosDatasetObjPtr			mInputDataset;

	//If have subquery, keep the subquery object
	//Currently, only keep one subquery. Going
	//forward, there could be many subQueries
	//
	//Now, we assume if there is a child query,
	//the input data for current query should 
	//be the result from the child query
	AosJqlStmtQuery				*mChild;
public:
	AosJqlStmtQuery();
	AosJqlStmtQuery(const OmnString &errmsg);
	AosJqlStmtQuery(AosJqlSelect *select);
	~AosJqlStmtQuery();

	virtual bool 				run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual bool 				runQuery(
									const AosRundataPtr &rdata,
									AosDatasetObjPtr &ds);

	virtual AosJqlStatement*	clone();
	virtual void 				dump();
	virtual OmnString			getErrmsg()const {return mErrmsg;}

	bool doExpr(const AosRundataPtr &rdata);

	bool 	procInto(const AosRundataPtr &rdata, 
				const AosXmlTagPtr &data,
				const vector<OmnString> &fields);

	bool 	intoOutfile(const AosRundataPtr &rdata,
				const AosXmlTagPtr &data,
				const vector<OmnString> &fields);

	bool	 setOutputFieldLis(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &data,
				vector<OmnString> &fields);

	void 	setExprs(AosJqlSelectFieldList* exprs);

	AosXmlTagPtr getQueryConf(const AosRundataPtr &rdata);

	AosXmlTagPtr getQueryRslt(
					const AosRundataPtr &rdata, 
					const AosXmlTagPtr &query_conf);

	void setSelect(AosJqlSelectPtr selectPtr)
	{
		mSelectPtr = selectPtr;
	}
	 
	AosJqlSelectPtr getSelect()
	{
		return mSelectPtr;
	}
	bool executeQuery(
			const AosXmlTagPtr &def,
			const AosDatasetObjPtr &inputDataset,
			AosDatasetObjPtr &dataset,
			const AosRundataPtr &rdata);

	bool setSubQuery();

private:
	bool checkTableNameExist(
		const AosRundataPtr &rdata,
		const OmnString &tableName);

};
#endif
