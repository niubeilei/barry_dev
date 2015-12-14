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
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "BitmapTreeMgr/BitmapTreeMgr.h"
#include "BitmapTreeMgr/BitmapTree.h"
#include "BitmapTreeMgr/Ptrs.h"

AosBitmapTreeMgr*    
AosBitmapTreeMgr::getSelf()
{
	return 0;
}

bool        
AosBitmapTreeMgr::config(const AosXmlTagPtr &config)
{
	return true;
}

AosBitmapTreeObjPtr 
AosBitmapTreeMgr::createTree(
					const AosRundataPtr &rdata,
					const u64 &id,
					const u32 &iil_level,
					const u32 &max_leaf_size)
{
	AosBitmapTreePtr ptr;
	return ptr;
}

AosBitmapTreeObjPtr	
AosBitmapTreeMgr::getBitmapTree(const u64 &id,
									  const u32 &max_leaf_size,
									  const AosRundataPtr &rdata)
{
	AosBitmapTreePtr ptr;
	return ptr;
}
