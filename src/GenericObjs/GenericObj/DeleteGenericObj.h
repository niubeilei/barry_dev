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
// 2014/10/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GenericObjs_GenericObj_CreateGenericObj_h
#define Aos_GenericObjs_GenericObj_CreateGenericObj_h

#include "GenericObjs/GenericMethod.h"

class AosMethodCreateGenericObj : public AosGenericMethod
{
	OmnDefineRCObject;

protected:

public:
	AosGenMdCreateGenericObj(const int version);
	~AosGenMdCreateGenericObj();

	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual bool	proc(
						AosRundata *rdata,
						const OmnString &obj_name, 
						const vector<AosExprObjPtr> &parms);

};
#endif

