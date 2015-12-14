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
// 12/09/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_SOMgr_h
#define Aos_SemanticObj_SOMgr_h

#include "SemanticObj/Ptrs.h"
#include "Util/HashObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSOMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnObjHash<AosSemanticObjPtr, 0xfff>	mObjs;

public:
	AosSOMgr();
	~AosSOMgr();

	bool	addSO(const AosSemanticObjPtr &so);
	bool	removeSO(const AosSemanticObjPtr &so);
};

#endif
