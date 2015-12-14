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
// Created: 08/04/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Book_TagMgr_h
#define Omn_Book_TagMgr_h

#include "Book/Ptrs.h"
#include "Obj/ObjDb.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include <string.h>

class AosSystemData;

class AosTagMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxTags = 100
	};

protected:

public:
	AosTagMgr(); 
	virtual ~AosTagMgr() {}

	bool 	procTags(
				const OmnString &tags, 
				const AosSystemData &data);
};

#endif

