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
#ifndef AOS_JQLStatement_JqlStmtMap_H
#define AOS_JQLStatement_JqlStmtMap_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtMap : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString		mMapName;
	OmnString		mTableName;
	OmnString 		mDataType;
	u32				mMaxLen;
	AosExprList		*mKeys;
	AosExprObj 		*mValue;
	AosExprObj		*mWhereCond;

public:
	OmnString		mErrmsg;

public:
	AosJqlStmtMap(const OmnString &errmsg);
	AosJqlStmtMap();
	~AosJqlStmtMap();

	//getter/setters
	void setMapName(OmnString name);
	void setTableName(OmnString name);
	void setKeys(AosExprList *key_list);
	void setValue(AosExprObj *value);
	void setDataType(AosJQLDataFieldTypeInfo *type);
	void setMaxLen(int len);
	void setWhereCond(AosExprObj *where_cond);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

private:
	bool createMap(const AosRundataPtr &rdata);  
	bool showMaps(const AosRundataPtr &rdata);   
	bool describeMap(const AosRundataPtr &rdata);
	bool dropMap(const AosRundataPtr &rdata);   
	bool listMap(const AosRundataPtr &rdata);   
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

};

#endif
