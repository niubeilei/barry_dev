////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TValue.h
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef OMN_Tester_TValue_h
#define OMN_Tester_TValue_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


template<class T>
class OmnTValue : public OmnRCObject
{
	OmnDefineRCObject;

public:
	OmnTValue() {}
	virtual ~OmnTValue() {}

	virtual T		nextValue(bool &status) = 0;
	virtual void	reset() = 0;
};

#endif

