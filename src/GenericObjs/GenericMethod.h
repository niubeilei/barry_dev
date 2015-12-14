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
//
// Modification History:
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_GenericMethod_h
#define Aos_GenericObjs_GenericMethod_h

#include "SEInterfaces/GenericMethodObj.h"
#include "Thread/Ptrs.h"


class AosGenericMethod : public AosGenericMethodObj
{
	OmnDefineRCObject;
protected:
	OmnString		mMethodName;

public:
	AosGenericMethod(const int version);
	~AosGenericMethod();

	virtual AosJimoPtr cloneJimo() const;
	virtual OmnString getMethodName() const {return mMethodName;}

	virtual bool config(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc);

	virtual bool proc(
			AosRundata *rdata, 
			const OmnString &obj_name, 
			const vector<AosExprObjPtr> &parms);
};
#endif

