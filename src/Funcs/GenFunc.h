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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Funcs_GenFunc_h
#define Aos_Funcs_GenFunc_h

#include "Jimo/JimoGenFunc.h"
#include "Util/String.h"


class AosGenFunc : public AosJimoGenFunc
{
protected:
	u32						mSize;
	OmnString				mFuncName;
	AosValueRslt			mValue;
	vector<AosExprObjPtr>	mParms;
	AosValueRslt			*mConstValues;
	AosExprObj**			mExprValues;

public:
	AosGenFunc(const OmnString &fname, const int version);
	AosGenFunc(const u32 type, const int version);
	AosGenFunc(const AosGenFunc &rhs);
	virtual ~AosGenFunc();

	// Andy, 2015/08/21
	virtual bool setParms(AosRundata *rdata, AosExprList *parms);

	// Young, 2015/05/29
	virtual AosDataType::E getDataType(
						AosRundata *rdata,
						AosDataRecordObj *record);

	//yang
	virtual bool getValue(
					AosRundata *rdata,
					const OmnString& keyValue,
					AosValueRslt &value)
	{
		return false;
	}


	// Chen Ding, 2014/09/16
	virtual bool getValue(
						AosRundata *rdata, 
						AosValueRslt &value, 
						AosDataRecordObj *record);

	// Chen Ding, 2014/09/16
	virtual bool syntaxCheck(
						AosRundata *rdata, 
						OmnString &errmsg);

	virtual bool getValue(
						AosRundata *rdata, 
						int	idx,
						AosDataRecordObj *record);
};
#endif



