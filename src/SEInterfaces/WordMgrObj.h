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
// 2013/03/11 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_WordMgrObj_h
#define Aos_SEInterfaces_WordMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"


class AosWordMgrObj : virtual public OmnRCObject
{
protected:
	static AosWordMgrObjPtr		smWordMgr;

public:
	
	static void setWordMgr(const AosWordMgrObjPtr &wordmgr) {smWordMgr = wordmgr;}
	static AosWordMgrObjPtr getWordMgr() {return smWordMgr;}

protected:
};

#endif

