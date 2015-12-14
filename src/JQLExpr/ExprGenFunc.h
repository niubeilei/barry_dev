////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprGenFunc_h
#define Aos_JQLExpr_ExprGenFunc_h

#include "Jimo/Ptrs.h"
#include "JQLExpr/ExprBinary.h"
#include "SEInterfaces/DataRecordObj.h"
#include <map>

class AosExprGenFunc : public AosExpr
{
private:
	static map<OmnString, OmnString>  sgFuncNamesMap;

	OmnString 				mFuncName;
	AosExprList*			mParms;
	AosExprObjPtr			mExpr;
	AosJimoGenFuncPtr       mJimoPtr;
	AosJimoGenFunc			*mJimo;
	bool					mIsAggr;
	
bool 					mTestTag;

public:
	AosExprGenFunc();

	AosExprGenFunc(const OmnString &fname,
				AosExprList  *parms);

	AosExprGenFunc(const OmnString &fname,
				AosExprObjPtr mExpr);

	virtual ~AosExprGenFunc();

	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata       *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getFields(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool getFieldsByStat(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool mergeTerms(
					const AosRundataPtr &rdata, 
					bool &merged, 
					AosExprObjPtr &merged_term);

	virtual OmnString getStatFieldName();

	virtual bool isExprCond() const;

	virtual OmnString getFuctName()const ;
	virtual AosExprList* getParmList() const;
	virtual AosExprObjPtr getFieldExpr() ;
	virtual int getNumFieldTerms() const;
	virtual bool fieldExprWithAggr() const ;
	virtual AosExprObjPtr cloneExpr() const;
	virtual OmnString dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	static void funcNamesMapInit();
	static AosXmlTagPtr createJimoDoc(
						const AosRundataPtr &rdata,
						OmnString class_name,
						//const OmnString &lib_name = "libQueryFuncJimos.so",
						const OmnString &lib_name = "libFuncJimos.so",
						const u32 ver = 1);

	u32 getParms(const AosRundataPtr &rdata, vector<AosExprObjPtr> &parms);

	AosExprType::E getType() const {return AosExprType::eGenFunc;}

	//2015/8/4 xuqi JIMODB-207
	bool init(const OmnString &fname, 
			AosExprObjPtr expr);
    
	bool init(const OmnString &fname,
			AosExprList *parms);

};
#endif

