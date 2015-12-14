////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjKernel.h
// Description:
//	This is the super class for all kernel objects. 
//  KernelObj is used to capture objects passed on from the kernel.
//  There can be many types of kernel objects, all should be
//  derived from SsnKernelObj.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_KernelObj_h
#define Omn_Obj_KernelObj_h

#include "Obj/Obj.h"


class OmnKernelObj : virtual public OmnObj
{
public:
	OmnKernelObj();
	virtual ~OmnKernelObj();

	virtual bool	serializeToKernel() const = 0;
    virtual bool    serializeFromKernel() = 0;
};
#endif
