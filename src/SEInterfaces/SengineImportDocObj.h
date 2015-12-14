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
// 2014/01/11	Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SengineImportDocObj_h
#define Aos_SEInterfaces_SengineImportDocObj_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosSengineImportDocObj : virtual public OmnRCObject
{
public:
	virtual void		allFinished() = 0;
	virtual void		semPost() = 0;
	virtual AosBuffPtr	getNextBlock(
					const u64 &reqId,
					const int64_t &expected_size) = 0;

	virtual bool 	storageEngineError(const AosRundataPtr &rdata) = 0;
};

#endif

