////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_MethodDef_h
#define Aos_Jimo_MethodDef_h

#include "Jimo/MethodId.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosJimo;

struct AosMethodDef
{
	AosMethodId::E	method_id;
	void *			func;
	bool			mandatory;

	AosMethodDef()
	:
	method_id(AosMethodId::eInvalidMethodId),
	func(0),
	mandatory(false)
	{
	}

	AosMethodDef(const AosMethodId::E m, void *f, const bool d)
	:
	method_id(m),
	func(f),
	mandatory(d)
	{
	}
};

// JimoBasicFunc001
// 	Parameters:
// 		rdata:		(AosRundataPtr)	The rdata. It cannot be null
// 		jimo:		(AosJimo *) Note that this is a raw pointer.
//  Description:
//  	This function assumes the jimo has been configured properly
//  	by the worker doc. 
typedef bool (*AosRundata_Jimo)(
			const AosRundataPtr &rdata, 
			AosJimo *jimo);

// JimoBasicFunc002
// 	Parameters:
// 		rdata:		(AosRundataPtr)	The rdata. It cannot be null
// 		jimo:		(AosJimo *) Note that this is a raw pointer.
// 		worker_doc:	(AosXmlTagPtr) This is the worker doc. 
// 	DescriptioN:
// 		This function assumes that it needs to use 'worker_doc' 
// 		to do the processing. 
typedef bool (*AosMethodType_Rundata_Jimo_WorkerDoc)(
			const AosRundataPtr &rdata, 
			AosJimo *jimo,
			const AosXmlTagPtr &worker_doc);

typedef bool (*AosMethodType_Rundata_WorkerDoc)(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &worker_doc);
#endif

