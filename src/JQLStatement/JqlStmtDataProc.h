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
#ifndef AOS_JQLStatement_JqlStmtDataProc_H
#define AOS_JQLStatement_JqlStmtDataProc_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtDataProc : public AosJqlStatement
{
public:
	
private:
	//data from JQLParser
	OmnString				mProcName;
	OmnString				mProcType;
	OmnString				mOutput;
	OmnString				mOutputAsName;
	OmnString				mDatasetName;
	OmnString				mDataRecordName;
	OmnString				mOperator;
	OmnString				mIILName;
	OmnString				mDocType;
	AosExprList*   			mInputs; 

public:
	AosJqlStmtDataProc();
	~AosJqlStmtDataProc();

	//getter/setters
	void setName(OmnString name);
	void setType(OmnString type);
	void setInputs(AosExprList* input);
	void setOutput(OmnString output);
	void setOutputAsName(OmnString name);
	void setDatasetName(OmnString dataset_name);
	void setDataRecordName(OmnString data_record_name);
	void setOperator(OmnString opt);
	void setIILName(OmnString iil_name);
	void setDocType(OmnString doc_type);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createDataProc(const AosRundataPtr &rdata);  
	bool showDataProcs(const AosRundataPtr &rdata);   
	bool describeDataProc(const AosRundataPtr &rdata);
	bool dropDataProc(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

};

#endif
