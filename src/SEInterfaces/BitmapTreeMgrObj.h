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
// 2013/01/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapTreeMgrObj_h
#define Aos_SEInterfaces_BitmapTreeMgrObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBitmapTreeMgrObj : public AosJimo
{
private:
	static AosBitmapTreeMgrObjPtr		smObject;

public:
	AosBitmapTreeMgrObj(const int version);

	static AosBitmapTreeMgrObjPtr getObject();
	

	virtual AosBitmapTreeObjPtr createTree(
					const AosRundataPtr &rdata,
					const u64 &id,
					const u32 &iil_level,
					const u32 &max_leaf_size) = 0;

	virtual AosBitmapTreeObjPtr	getBitmapTree(const u64 &id,
									  const u32 &max_leaf_size,
									  const AosRundataPtr &rdata) = 0;

	virtual bool saveBitmapTree(
				const AosRundataPtr &rdata,
				const AosBitmapTreeObjPtr &tree) = 0;

	virtual bool saveBitmapTree(
				const AosRundataPtr &rdata,
				const u64 &tree_id,
				const AosBuffPtr &buff) = 0;

private:
	
/*	virtual bool saveTree(
					const u64 &bitmap_id,
					const AosBuffPtr &buff, 
					const AosRundataPtr &rdata) = 0;

	virtual AosBitmapTreeObjPtr retrieveTree(
					const u64 &bitmap_id,
					const AosRundataPtr &rdata) = 0;
*/
};

inline AosBitmapTreeMgrObjPtr AosGetBitmapTreeMgr()
{
	return AosBitmapTreeMgrObj::getObject();
}

#endif
