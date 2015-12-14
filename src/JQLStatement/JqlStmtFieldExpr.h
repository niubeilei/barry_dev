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
#ifndef AOS_JQLStatement_JqlStmtFieldExpr_H
#define AOS_JQLStatement_JqlStmtFieldExpr_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/DataFieldType.h"
#include "Util/String.h"

class AosJqlStmtFieldExpr : public AosJqlStatement
{
public:
	
private:
	OmnString				mFieldName;
	OmnString				mFieldTypeStr;
	OmnString				mDataType;
	OmnString				mMaxLen;
	OmnString				mOffset;
	OmnString 				mTableName;
	OmnString 				mExprStr;


public:
	AosJqlStmtFieldExpr();
	~AosJqlStmtFieldExpr();

	//getter/setters
	void setName(OmnString name);
	void setType(OmnString type);
	void setDataType(AosJQLDataFieldTypeInfo* type);
	void setMaxLen(int64_t len);
	void setOffset(int64_t len);
	void setTable(OmnString name);
	void setExpr(AosExprObj* expr);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataField(const AosRundataPtr &rdata);  
	bool showDataFields(const AosRundataPtr &rdata);   
	bool describeDataField(const AosRundataPtr &rdata);
	bool dropDataField(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	static OmnString getObjid(OmnString const name);
};

#endif
