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
#ifndef AOS_JQLStatement_JqlStmtRunScriptFile_H
#define AOS_JQLStatement_JqlStmtRunScriptFile_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtRunScriptFile : public AosJqlStatement
{
private:
	//data from JQLParser
	OmnString			mFileName;
	OmnString 			mErrorMsg;
	bool				mSuppressFlag;
	AosExprList			*mParms;
public:
	AosJqlStmtRunScriptFile();
	~AosJqlStmtRunScriptFile();

	//getter/setters
	void setFileName(OmnString name);
	bool runCmd(const OmnString fname, const AosRundataPtr &rdata);
	bool isIgnore(OmnString cmd);
	bool isSourceCmd(const OmnString cmd);
	OmnString getSourceFileName(const OmnString source_cmd);
	OmnString parser_data(OmnString &data);

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setSuppress(bool suppress_flag);
	bool getSuppress();
	void setParameters(AosExprList *parms);

private:
	void setParametersToAosJQLParser();
	OmnString getNextCmd(
			vector<OmnString> &cmdV,
			bool &finish,
			u32 &offset,
			OmnString &allCmdResp,
			const AosRundataPtr &rdata);
	
	bool collectJob(bool &isCollect,OmnString &stmt, OmnString &tmpjob);
};

#endif
