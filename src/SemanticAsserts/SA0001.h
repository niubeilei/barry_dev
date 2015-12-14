////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticAsserts_SA0001_h
#define Aos_SemanticAsserts_SA0001_h

#include "SemanticAsserts/SemanticAssert.h"
#include "SemanticUtil/MemAllocType.h"
#include "Util/RCObjImp.h"

class AosSA0001 : virtual public AosSemanticAssert
{
	OmnDefineRCObject;

private:
	void *				mPointer;
	void *				mPointedTo;
	AosMemAllocType		mMemAllocType;


public:
	AosSA0001(void *pointer, 
			  void *pointedTo,
			  const AosMemAllocType allocType, 
			  AosConditionList &conditions,
			  AosEventList &events);
	~AosSA0001();

};

#endif
