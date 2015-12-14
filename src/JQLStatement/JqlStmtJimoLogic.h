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
// 08/18/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtJimoLogic_H
#define AOS_JQLStatement_JqlStmtJimoLogic_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtJimoLogic : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString 			mLogicName;
	OmnString 			mAsName;
	OmnString 			mTableName;
	OmnString 			mEndPoint;
	OmnString 			mTime;
	OmnString			mCheckPoint;
	OmnString 			mMatrixName;
	OmnString 			mMatrix2Name;
	OmnString 			mResultsName;

public:
	AosJqlStmtJimoLogic();
	~AosJqlStmtJimoLogic();

	//getter/setters
	void setName(OmnString name);
	void setAsName(OmnString name);
	void setTableName(OmnString name);
	void setEndPoint(OmnString name);
	void setTime(OmnString name);
	void setCheckPoint(OmnString name);
	void setMatrixName(OmnString name);
	void setSecondName(OmnString name);
	void setResultsName(OmnString name);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createLogic(const AosRundataPtr &rdata);  
	bool showLogics(const AosRundataPtr &rdata);   
	bool describeLogic(const AosRundataPtr &rdata);
	bool dropLogic(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);
	static OmnString getObjid(OmnString const name);
};

#endif
