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
// 2015/01/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef OMN_SEInterfaces_TestMgrObj_h
#define OMN_SEInterfaces_TestMgrObj_h

#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"


class AosRundata;

class AosTestMgrObj : public OmnRCObject
{
private:
	static AosTestMgrObjPtr smTestMgr;

public:
	virtual ~AosTestMgrObj();

	virtual bool 	runTorturer(AosRundata *rdata, const OmnString &jimo_name) = 0;
	virtual void 	setTries(const int t) = 0;
	virtual int		getTries() const = 0;
	virtual void	setSeed(const int seed) = 0;

	static AosTestMgrObjPtr getTestMgr() {return smTestMgr;}
	static void setTestMgr(const AosTestMgrObjPtr &mgr) {smTestMgr = mgr;}
};

#endif

