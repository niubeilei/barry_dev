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
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcJoin_h
#define AOS_JimoLogicNew_JimoLogicDataProcJoin_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcJoin : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString			mJoin;
	vector<OmnString>	mLeftCondFields;
	vector<OmnString>	mRightCondFields;

public:

	AosJimoLogicDataProcJoin(const int version);
	~AosJimoLogicDataProcJoin();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					AosJqlStatementPtr &statement);

	//AosJqlStatement interface
	
	virtual bool run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str); 
	
	virtual bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

private:
	
	bool createJoinDataProc(
					AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str);
	
	bool addJoinTask(
				AosRundata *rdata, 
				AosJimoProgObj *prog);

};
#endif

