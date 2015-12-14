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
// 08/05/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_DfmCaller_h
#define Aos_DocFileMgr_DfmCaller_h

#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosDfmCaller : virtual public OmnRCObject
{
public:
	virtual void readCallBack(const u64 &docid, const AosDfmDocPtr &doc) = 0;
	virtual void readError(const u64 &docid, OmnString &errmsg) = 0;

	virtual void saveCallBack(const u64 &docid) = 0;
	virtual void saveError(const u64 &docid, OmnString &errmsg) = 0;

	virtual void deleteCallBack(const u64 &docid) = 0;
	virtual void deleteError(const u64 &docid, OmnString &errmsg) = 0;
};
#endif

