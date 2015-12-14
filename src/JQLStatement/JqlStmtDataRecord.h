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
#ifndef AOS_JQLStatement_JqlStmtDataRecord_H
#define AOS_JQLStatement_JqlStmtDataRecord_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDataRecord : public AosJqlStatement
{

public:
	OmnString		mErrmsg;
	
private:
	//data from JQLParser
	OmnString		mName;
	OmnString 		mType;
	OmnString		mRecordLen;
	OmnString		mRecordType;
	OmnString		mTrimCondition;
	AosExprObj*		mSchemaPicker;
	OmnString 		mFieldDelimiter;
	OmnString		mRecordDelimiter;
	OmnString		mTextQualidier;
	AosExprList*  	mFieldNemes; 

public:
	AosJqlStmtDataRecord(const OmnString &errmsg);
	AosJqlStmtDataRecord();
	~AosJqlStmtDataRecord();

	//getter/setters
	void setName(OmnString name);
	void setType(OmnString type);
	void setRecordType(OmnString type);
	void setRecordLength(int64_t len);
	void setTrimCondition(OmnString name);
	void setScheamPicker(AosExprObj *exprobj);
	void setFieldNames(AosExprList *fieldnames);
	void setFieldDelimiter(OmnString name);
	void setRecordDelimiter(OmnString name);
	void setTextQualidier(OmnString name);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataRecord(const AosRundataPtr &rdata);  
	bool showDataRecords(const AosRundataPtr &rdata);   
	bool describeDataRecord(const AosRundataPtr &rdata);
	bool dropDataRecords(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertMutilRecordConf(const AosRundataPtr &rdata);
	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

};

#endif
