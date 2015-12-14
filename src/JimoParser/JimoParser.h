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
// 2015/03/26 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoParser_JimoParser_H
#define AOS_JimoParser_JimoParser_H

#include "JQLStatement/Ptrs.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/JQLParserObj.h"
#include "SEInterfaces/GenericObj.h"
#include "SEInterfaces/ExprObj.h"
#include "SEInterfaces/Ptrs.h"
#include "JimoParser/Ptrs.h"
#include "Util/Charset.h"
#include "Util/String.h"
#include <vector>


class AosParserlet;
class OmnMutex;
class AosRundata;

class AosJimoParser :public  AosJimoParserObj,
					 public  AosGenericObj
{
	OmnDefineRCObject;

private:

	AosParserletPtr		 		mRootParserlet;
	OmnString					mJimoName;
	OmnString					mGenName;
	OmnString					mObjName;
	OmnString					mOrigStmt;
	char *						mData;	
	int				   			mDataLen;
	int							mCrtIdx;

	OmnMutex *					mLock;
	bool						mDebugFlag;
	AosCharset					mNameCharset;
	AosCharset					mKeywordCharset;
	vector<AosExprObjPtr> 		mNameValues;
	vector<AosExprObjPtr>::iterator		itr;


	//arvin 2015.7.7
	OmnString 					mErrmsg;
	AosCharset 					mExprCharset;

public:
	AosJimoParser(AosRundata *rdata,const int version);
	AosJimoParser(const AosJimoParser &parser);
	~AosJimoParser();

	virtual bool parse(
			AosRundata *rdata, 
			AosJimoProgObj *prog,
			const OmnString &stmt, 
			vector<AosJqlStatementPtr> &statements,
			bool dft);

	virtual AosJimoParserObjPtr createJimoParser(AosRundata *rdata);

	// GenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	virtual bool runJQL(
					AosRundata *rdata, 
					const AosJimoProgObjPtr &job,
					const OmnString &verb_name);

	virtual OmnString getObjType(AosRundata *rdata);
	virtual bool setJimoName(const OmnString &name)
	{
		mJimoName = name;
		return true;
	}
	virtual OmnString getJimoName() const {return mJimoName;}

	// Jimo interface
	AosJimoPtr cloneJimo() const;

	// JimoParser interface
	// arvin 2015/3/31
	//
	virtual bool nextStatement(
			AosRundata *rdata,
			AosJimoProgObj *prog,			// Xia Fan, 2015/12/05
			OmnString &expr_str,
			AosJqlStatementPtr &statement,
			bool dft);

	virtual bool peekNextChar(const char character);

	virtual bool parseProcedureParms(AosRundata *rdata,vector<OmnString> &parms);

	virtual bool getNameValueList(AosRundata *rdata, vector<AosExprObjPtr> &name_values); 

	virtual OmnString getParmStr(
			AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values);

	//arvin 2015.7.7
	virtual OmnString nextExpr(AosRundata *rdata);

	virtual int		  getParmInt(
			AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values);

	virtual AosExprObjPtr getParmExpr(
			AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values);

	virtual bool parmExist(AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values);

	virtual bool getParmArray(AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values, 
			vector<AosExprObjPtr> &keys);

	virtual bool getParmArrayStr(AosRundata *rdata, 
			const OmnString &parm, 
			vector<AosExprObjPtr> &name_values, 
			vector<OmnString> &parmArray);

	virtual bool nextSemiColon(AosRundata *rdata);

	OmnString nextKeyword(AosRundata *rdata);

	virtual OmnString nextObjName(AosRundata *rdata);

	virtual OmnString getOrigStmt()
	{
		return mOrigStmt;
	}

	virtual void setOrigStmt(const OmnString stmt)
	{
		mOrigStmt = stmt;
		mData = (char*)mOrigStmt.data();
		mDataLen = stmt.length();
	}

private:
	bool parseOneJQLStatement(AosRundata *rdata, 
			vector<AosJqlStatementPtr> &statements,
			const int start_idx);

	bool getGenericObjsLocked(AosRundata *rdata,
			vector<AosJimoParserObjPtr> &genobjs);

	bool registerParserlet(
			AosRundata *rdata, 
			const OmnString &parserlet_name,
			const OmnString &jimo_name, 
			const int version,
			bool &override_flag);


	OmnString nextWordPriv(AosRundata *rdata, const AosCharset &charset);

	OmnString nextProcdureParm(AosRundata *rdata);	
	bool parseNameValueList(AosRundata *rdata, 
			vector<AosExprObjPtr> &name_values);

	bool config(AosRundata *rdata);

	bool getNameValueListStr(
			AosRundata *rdata,
			int &start_index, 
			int &end_index);

};

#endif
