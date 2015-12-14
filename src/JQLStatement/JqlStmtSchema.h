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
#ifndef AOS_JQLStatement_JqlStmtSchema_H
#define AOS_JQLStatement_JqlStmtSchema_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtSchema : public AosJqlStatement
{
private:
	AosXmlTagPtr 					mDoc;
	OmnString						mComment;

	//data from JQLParser
	OmnString						mSchemaType; //static or dynamic
	OmnString						mRecordType;
	OmnString						mName;
	OmnString						mType;
	OmnString						mRecord;
	OmnString						mLineBreak;
	vector<AosJqlRecordPickerPtr>    *mPickers;

	//for dropping purpose
	AosExprList						*mSchemaNames;

public:
	OmnString						mErrmsg;

	// multi record
	AosExprList						*mRecordNames;
	u32								mMaxRecordLen;
	OmnString						mDelimiter;
	bool							mIsIgnoreSubPattern;
	bool							mIsSkipInvalidRecords;
	u32								mSchemaPos;
	u32								mSchemaLen;

public:
	AosJqlStmtSchema(const OmnString &errmsg);
	AosJqlStmtSchema();
	~AosJqlStmtSchema();

	//getter/setters
	void setSchemaType(OmnString schemaType);
	void setRecordType(OmnString recordType);
	void setName(OmnString name);
	void setType(OmnString type);
	void setRecord(OmnString record);
	void setLineBreak(OmnString lineBreak);
	void setPickers(vector<AosJqlRecordPickerPtr> *pickers);
	void setComment(OmnString comment);

	// multi record
	void setRecordNames(AosExprList *names);
	void setRecordLen(u32 len);
	void setRecordDelimiter(OmnString delimiter);
	void setIgnoreSubPattern(bool rslt);
	void setSkipInvalidRecords(bool rslt);
	void setSchemaPos(u32 pos);
	void setSchemaLen(u32 len);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	bool createSchema(const AosRundataPtr &rdata);
	bool showSchemas(const AosRundataPtr &rdata);
	bool describeSchema(const AosRundataPtr &rdata);
	bool dropSchema(const AosRundataPtr &rdata);

	AosXmlTagPtr generateMultiRecordConf(const AosRundataPtr &rdata);

};

#endif
