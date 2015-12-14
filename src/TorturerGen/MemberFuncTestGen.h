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
// 02/19/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TorturerGen_MemberFuncTestGen_h
#define Omn_TorturerGen_MemberFuncTestGen_h

#include "RVG/Ptrs.h"
#include "TorturerGen/TestGen.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlParser/Ptrs.h"



class AosMemberFuncTestGen : public AosTestGen
{
	OmnDefineRCObject;

protected:

public:
	AosMemberFuncTestGen();
	virtual ~AosMemberFuncTestGen();

	virtual OmnString	genCode(const OmnString &classname);

	bool	config(const OmnXmlItemPtr &def);

private:
};

#endif

