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
// This class is different from AosVariable in that it has a value
// mapper. It can map values from one set to another set.
//
// Modification History:
// 07/27/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TorturerUtil_VariableWithMapper_h
#define AOS_TorturerUtil_VariableWithMapper_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"


class AosVariableWithMapper : virtual OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosVariableWithMapper();
	~AosVariableWithMapper();

	virtual bool	getValue() const;
};

#endif

