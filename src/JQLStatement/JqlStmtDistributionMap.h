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
// 2015/05/04 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtDistributionMap_H
#define AOS_JQLStatement_JqlStmtDistributionMap_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDistributionMap : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mMapName;
	OmnString		mTableName;
	OmnString 		mDataType;
	u32				mMaxLen;
	u32				mMaxNums;
	u32				mSpliteNum;
	AosExprList		*mKeys;
	AosExprObj		*mWhereCond;

public:
	OmnString		mErrmsg;

public:
	AosJqlStmtDistributionMap(const OmnString &errmsg);
	AosJqlStmtDistributionMap();
	~AosJqlStmtDistributionMap();

	//getter/setters
	void setMapName(OmnString name);
	void setTableName(OmnString name);
	void setKeys(AosExprList *key_list);
	void setDataType(OmnString data_type);
	void setMaxLen(int len);
	void setMaxNums(int nums);
	void setSpliteNum(int splite);
	void setWhereCond(AosExprObj *where_cond);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createMap(const AosRundataPtr &rdata);  
	bool showMaps(const AosRundataPtr &rdata);   
	bool describeMap(const AosRundataPtr &rdata);
	bool dropMap(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	static OmnString getObjid(OmnString const name);
};

#endif
