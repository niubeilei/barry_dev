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
// 2013/09/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLPARSER_JQLParser_H
#define AOS_JQLPARSER_JQLParser_H

#include "SeReqProc/SeRequestProc.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/Ptrs.h"
#include "SEInterfaces/ExprObj.h"
#include "JQLStatement/JQLCommon.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include <stdarg.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include "SEUtil/JqlTypes.h"

using namespace std;
struct AosJqlStatement;

extern int AosJQLParser_ReadInput(char *buffer, int *numBytesRead, int maxBytesToRead);

class AosJQLParser
{
private:
	OmnMutexPtr	mLock;
	OmnString	mContents;
	int 		mOffset;
	OmnString 	mErrmsg;
	AosExprObjPtr  mExpr;

	// static std::vector<AosJqlStatementPtr> smStatements;
	vector<AosJqlStatementPtr> 		mStatements;
	vector<AosJqlStatementPtr>		mStatements_bak;
	vector<AosExprObjPtr>	  		mSourceParms;
	map<OmnString, AosExprObjPtr>	mUserVar;

public:
	AosJQLParser();
	//AosJQLParser(const OmnString contentFormat);
	~AosJQLParser();

	void lock() {mLock->lock();}
	void unlock() {mLock->unlock();}

	bool parseWithRegex(const OmnString &contents, const AosRundataPtr &rdata);
	// bool run(const AosRundataPtr &rdata);
	// bool run(const AosRundataPtr &rdata, const OmnString type);
	bool setContents(char * data) 
	{ 
		//mLock->lock();
		mContents = data; 
		return true;
	}

	void saveStatements()
	{
		mStatements_bak = mStatements;
	}

	void recoverStatements()
	{
		mStatements = mStatements_bak;
	}

	void finishParse()
	{
		mContents = "";
		mStatements.clear();
	}

	void cleanSourceParms()
	{
		mSourceParms.clear();
	}

	void cleanUserVar()
	{
		mUserVar.clear();
	}

	bool reset();
	bool setExpr(AosExprObjPtr expr){mExpr = expr; return true;}
	bool setUserVar(const OmnString &name, const AosExprObjPtr &value);
	bool setSourceParms(const AosExprObjPtr &value);

	AosExprObjPtr getUserVar(const OmnString &name);
	AosExprObjPtr getSourceParm(const int index);

	void eraseUserVar(const OmnString &name);

	AosExprObjPtr getExpr(){mStatements.clear(); return mExpr;}
	int  getContents(char* buffer, int *numBytesRead, int maxBytesToRead);
	void appendStatement(const AosJqlStatementPtr &statement);
	AosJqlStatementPtr getStatement();

	void dump();
	void clearStatements() {mStatements.clear();}

	// Chen Ding, 2015/01/29
	vector<AosJqlStatementPtr> getStatements() 
	{
		vector<AosJqlStatementPtr> statements = mStatements;
		mStatements.clear();
		return statements;
	}
	OmnString getErrmsg() const {return mErrmsg;}
};

#endif
