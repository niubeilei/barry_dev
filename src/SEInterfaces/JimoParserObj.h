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
// 2015/03/25 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JimoParserObj_h
#define AOS_SEInterfaces_JimoParserObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/Ptrs.h"
#include "JQLExpr/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosJimoParserObj : virtual public AosJimo
{

protected:
	vector<OmnString> mKeywords;

public:
	AosJimoParserObj(const int version);

	AosJimoParserObj();

	virtual AosJimoParserObjPtr createJimoParser(AosRundata *rdata) = 0;

	virtual bool parse(
				AosRundata *rdata, 
				AosJimoProgObj *prog,
				const OmnString &stmt, 
				vector<AosJqlStatementPtr> &statements,
				bool dft = false) = 0;

	virtual bool getNameValueList(AosRundata *rdata, vector<AosExprObjPtr> &name_values) = 0;

	virtual bool parseProcedureParms(AosRundata *rdata,vector<OmnString> &parms) = 0;

	virtual OmnString getParmStr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) = 0 ;

	virtual int		  getParmInt(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) = 0 ;

	virtual AosExprObjPtr getParmExpr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values)= 0;

	virtual bool parmExist(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values)= 0;

	virtual bool getParmArray(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values, 
				vector<AosExprObjPtr> &keyname) = 0;

	virtual bool getParmArrayStr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values, 
				vector<OmnString> &measure) = 0;

	virtual bool nextSemiColon(AosRundata *rdata) = 0;

	virtual OmnString nextObjName(AosRundata *rdata) = 0;

	virtual bool peekNextChar(const char character) = 0;

	virtual bool nextStatement(
			AosRundata *rdata,
			AosJimoProgObj *prog,			// Xia Fan, 2015/12/05
			OmnString &expr_str, 
			AosJqlStatementPtr &statement,
			bool dft = false) = 0;	

	virtual OmnString nextKeyword(AosRundata *rdata) = 0;
	
	virtual OmnString nextExpr(AosRundata *rdata) = 0;

	virtual OmnString getOrigStmt() = 0;
	
	virtual void setOrigStmt(const OmnString stmt) = 0;

	void setKeywords(const vector<OmnString> &keywords) {mKeywords = keywords;}

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,			// Xia Fan, 2015/12/05
					bool &parsed, 
					bool dft = false) = 0;

};
#endif

