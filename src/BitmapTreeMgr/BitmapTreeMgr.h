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
#ifndef Aos_BitmapTreeMgr_BitmapTreeMgr_h
#define Aos_BitmapTreeMgr_BitmapTreeMgr_h

#include "SEInterfaces/BitmapTreeMgrObj.h"

class AosBitmapTreeMgr : virtual public AosBitmapTreeMgrObj 
{

public:
	static AosBitmapTreeMgr*    getSelf();
	virtual bool        config(const AosXmlTagPtr &config);
	virtual AosBitmapTreeObjPtr createTree(
					const AosRundataPtr &rdata,
					const u64 &id,
					const u32 &iil_level,
					const u32 &max_leaf_size);

	virtual AosBitmapTreeObjPtr	getBitmapTree(const u64 &id,
									  const u32 &max_leaf_size,
									  const AosRundataPtr &rdata);

	virtual bool saveBitmapTree(
				const AosRundataPtr &rdata,
				const AosBitmapTreeObjPtr &tree){return true;}
	
};

#endif
