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
#ifndef Aos_SEInterfaces_GenericObjMgrObj_h
#define Aos_SEInterfaces_GenericObjMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosGenericObjMgrObj : public AosJimo
{
protected:

public:
	AosGenericObjMgrObj(const int version);
	~AosGenericObjMgrObj();

	static AosGenericObjMgrObjPtr getSelf(AosRundata *rdata);

	virtual AosGenericMethodObjPtr getGenericObj(
				AosRundata *rdata, 
				const OmnString &verb, 
				const OmnString &objname, 
				const int version) = 0;

	virtual AosGenericMethodObjPtr createGenericObj(
				AosRundata *rdata, 
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc) = 0;

	virtual bool proc(
				AosRundata *rdata,
				const AosJimoProgObjPtr &prog,
				const OmnString &verb_name,
				const OmnString &obj_type,
				const OmnString &obj_name,
				const OmnString &json) = 0;
				
};
#endif

