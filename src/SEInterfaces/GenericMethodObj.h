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
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_GenericMethodObj_h
#define Aos_SEInterfaces_GenericMethodObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosGenericMethodObj : public AosJimo
{
	static  AosGenericMethodObjPtr smGenericMethodObj;

public:
	AosGenericMethodObj(const int version);

	virtual OmnString getMethodName() const = 0;
	virtual AosJimoPtr cloneJimo() const = 0;

	virtual bool config(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc) = 0;

	virtual bool proc(
			AosRundata *rdata, 
			const OmnString &obj_name, 
			const OmnString &jsonstr) = 0;
};
#endif

