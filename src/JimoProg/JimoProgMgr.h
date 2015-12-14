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
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoProg_JimoProgMgr_h
#define AOS_JimoProg_JimoProgMgr_h

#include "JimoProg/Ptrs.h"
#include "SEInterfaces/JimoProgObj.h"
#include "Thread/Ptrs.h"
#include "API/AosApi.h"
#include "SEInterfaces/JimoProgMgrObj.h"
#include "UtilData/SysTableNames.h"
#include <vector>
#include <hash_map>


class AosJimoProgMgr : public AosJimoProgMgrObj
{
	OmnDefineRCObject;

protected:
	typedef hash_map<const OmnString, AosJimoProgObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosJimoProgObjPtr, Omn_Str_hash, compare_str>::const_iterator itr_t;

	map_t				mJimoProgMap;
	OmnMutexPtr         mLock;

public:
	AosJimoProgMgr(const int version);
	virtual ~AosJimoProgMgr();

	virtual	AosJimoPtr cloneJimo() const;
	virtual AosJimoProgObjPtr getJimoProg(AosRundata *rdata, const OmnString &name);
	virtual AosJimoProgObjPtr retrieveJimoProg(AosRundata *rdata, const OmnString &name) const;
};
#endif

