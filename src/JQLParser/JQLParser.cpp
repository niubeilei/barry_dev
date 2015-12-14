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
#include "JQLParser/JQLParser.h"


#include "JQLParser/ParserInclude.h"
#include "JQLParser/Parser.lex.cpp"
#include "Parser.tab.hh"
#include "SEServer/SeReqProc.h"
#include "Util/String.h"
#include <stdarg.h>
#include <cstdlib>
#include <vector>

#include <boost/regex.hpp>
using namespace std;
extern AosJQLParser gAosJQLParser;

int 
AosJQLParser_ReadInput(char* buffer, int *numBytesRead, int maxBytesToRead)
{
//	vector<AosJqlStatementPtr> statements;
//	if (0) AosParseJQL(0, 0, statements);
	return gAosJQLParser.getContents(buffer, numBytesRead, maxBytesToRead);
}


//yang 2015-08-03
vector<string> gTokenBuf;
void getToken(const char* token)
{
	gTokenBuf.push_back(token);
}


//yang 2015-08-03
OmnString getErrMsg()
{
	OmnString errmsg = "[ERR] : syntax error near \"";

	if(gTokenBuf.size()>10)
	{
		//yangyang 2015.08.05
		//JIMODB-268
		for(int i=gTokenBuf.size()-5;i<gTokenBuf.size();i++)
		{
			errmsg += gTokenBuf[i];
			errmsg += " ";
		}
	}
	else
	{
		for(int i=0;i<gTokenBuf.size();i++)
		{
			errmsg += gTokenBuf[i];
			errmsg += " ";
		}
	}
	errmsg += "\"!";
	return errmsg;
}


AosJQLParser::AosJQLParser()
{
	mLock = OmnNew OmnMutex();
 	mOffset = 0;
}


AosJQLParser::~AosJQLParser()
{
	//if (mExpr) delete mExpr;
}


/*
bool
AosJQLParser::run(const AosRundataPtr &rdata)
{
	bool rslt = false;
	if (mErrmsg != "") 
	{
		AosSetEntityError(rdata, "JQLParser_",
				       mErrmsg, mErrmsg) << enderr;               
		return false;
	}

	for (u32 i=0; i<mStatements.size(); i++)
	{
		rslt = mStatements[i]->run(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosJQLParser::run(const AosRundataPtr &rdata, const OmnString type)
{
	if (mErrmsg != "") 
	{
		AosSetEntityError(rdata, "JQLParser_",
				       mErrmsg, mErrmsg) << enderr;               
		return false;
	}

	for (u32 i=0; i<mStatements.size(); i++)
	{
		mStatements[i]->setContentFormat(type);
		mStatements[i]->run(rdata);
	}
	return true;
}
*/


void
AosJQLParser::appendStatement(const AosJqlStatementPtr &statement)
{
	mErrmsg = statement->getErrmsg();
	//if (mErrmsg != "") return;
	mStatements.push_back(statement);
}

void
AosJQLParser::dump()
{
	std::cout << "Statements parsed: " << mStatements.size() << std::endl;

	for (u32 i=0; i<mStatements.size(); i++)
	{
		mStatements[i]->dump();
	}
}


AosJqlStatementPtr
AosJQLParser::getStatement()
{
	if (mStatements.size() == 1)
	{
		AosJqlStatementPtr stmt =  mStatements[0];
		mStatements.clear();
		return stmt;
	}
	return 0;
}

int
AosJQLParser::getContents(char* buffer, int *numBytesRead, int maxBytesToRead)
{
	if (mContents == "")
	{
		*numBytesRead = 0;
		return 0;
	}

	char * input_data = mContents.getBuffer();
	int numBytesToRead = maxBytesToRead;
	int bytesRemaining = mContents.length() - mOffset;
	int i;
	if (numBytesToRead > bytesRemaining) 
	{ 
		numBytesToRead = bytesRemaining; 
	}

	for ( i = 0; i < numBytesToRead; i++ ) 
	{
		buffer[i] = input_data[mOffset+i];
	}

	*numBytesRead = numBytesToRead;
	mOffset+= numBytesToRead;

	if (numBytesToRead >= bytesRemaining) 
	{
		mOffset = 0;
		gAosJQLParser.reset();
	}
	return 0;
}


bool
AosJQLParser::reset()
{
	mContents = "";
	return true;
}


bool 
AosJQLParser::parseWithRegex(const OmnString &contents, const AosRundataPtr &rdata)
{
	if (contents == "") return false;

	//const char* szReg = "(\\w+)\\s+(\\w+)\\s+['\"]{0,1}(\\w+)['\"]{0,1}(\\s*;|\\s+(\\{[\\s\\S]*\\})\\s*;)";
	const char* szReg = "(\\w+)\\s+(\\w+)\\s+['\"]{0,1}([\\w\\.]+)['\"]{0,1}(\\s*;|\\s+(\\{[\\s\\S]*\\})\\s*;)";

	bool rslt = false;
	try 
	{
		boost::regex reg(szReg);

		boost::cmatch mat;
		rslt = boost::regex_match( contents.data(), mat, reg );
		if (rslt)
		{
			if (mat.size() != 6) return false;
			AosJqlStmtGenJQL* stmt = new AosJqlStmtGenJQL;

			for (size_t i=0; i<mat.size(); i++)
			{
				cout << "???????????REGEX[" << i << "]=" << mat[i] << endl; 
			}
			
			stmt->setVerbName(mat[1]);
			stmt->setObjectType(mat[2]);
			stmt->setObjectName(mat[3]);
			stmt->setJQLConfig(mat[5]);
			appendStatement(stmt);
		}
	}
	catch (std::exception &e)
	{
		OmnScreen << e.what() << endl;
	}
	return rslt;
}


bool 
AosJQLParser::setUserVar(
		const OmnString &name,
		const AosExprObjPtr &expr)
{
	mUserVar[name] = expr;
	return true;
}

bool 
AosJQLParser::setSourceParms(const AosExprObjPtr &expr)
{
	mSourceParms.push_back(expr);
	return true;
}


AosExprObjPtr
AosJQLParser::getUserVar(const OmnString &name)
{
	if (mUserVar.count(name) != 0)
	{
		return mUserVar[name];
	}
	return NULL;
}


AosExprObjPtr
AosJQLParser::getSourceParm(const int index)
{
	if ((int64_t)mSourceParms.size() > index)
	{
		return mSourceParms[index];
	}
	return NULL;
}


void 
AosJQLParser::eraseUserVar(const OmnString &name)
{
	if (mUserVar.count(name) != 0)
	{
		mUserVar.erase(name);
	}
}

