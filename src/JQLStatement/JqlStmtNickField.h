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
#ifndef AOS_JQLStatement_JqlStmtNickField_H
#define AOS_JQLStatement_JqlStmtNickField_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/DataFieldType.h"
#include "Util/String.h"

class AosJqlStmtNickField : public AosJqlStatement
{
private:
	OmnString				mFieldName;
	OmnString				mFieldTypeStr;
	OmnString 				mTableName;
	OmnString 				mOrigFieldName;
	int 					mMaxLen;


public:
	AosJqlStmtNickField();
	~AosJqlStmtNickField();

	//getter/setters
	void setName(OmnString name);
	void setTable(OmnString name);
	void setOrigFieldName(OmnString name);
	void setMaxLen(int len);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataField(const AosRundataPtr &rdata);  
	bool showDataFields(const AosRundataPtr &rdata);   
	bool describeDataField(const AosRundataPtr &rdata);
	bool dropDataFields(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

};

#endif
