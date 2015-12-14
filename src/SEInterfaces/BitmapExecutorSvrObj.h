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
// 2014/04/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapExecutorSvrObj_h
#define Aos_SEInterfaces_BitmapExecutorSvrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBitmapExecutorSvrObj : virtual public OmnRCObject
{
private:
	static AosBitmapExecutorSvrObjPtr		smObject;

public:
	virtual vector<AosBitmapObjPtr> &getBitmaps(
				const AosRundataPtr &rdata, 
				const u64 &bitmap_id) = 0;

	virtual AosBmpBlockIndexObjPtr	getNode(
				const AosRundataPtr &rdata,
			 	const u64 &node_id, 
				const int iil_level,
				const int node_level,
				const bool create_flag) = 0;

	virtual bool saveAllBlockIndex(const AosRundataPtr &rdata) = 0;

	virtual bool removeNodeFromDisk(
				const AosRundataPtr &rdata,
			 	const u64 &node_id, 
				const int iil_level,
				const int node_level) = 0;
};
#endif
