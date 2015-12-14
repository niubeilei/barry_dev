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
// 2013/02/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapEngine_BitmapExecutor_h
#define Aos_BitmapEngine_BitmapExecutor_h

#include "SEInterfaces/IILExecutorObj.h"
#include <vector>
using namespace std;


class AosBitmapExecutor: virtual public AosIILExecutorObj
{
public:
	virtual u64 createTempBitmap(
				const AosRundataPtr &rdata,
				const AosBitmapObjPtr &bitmap){return 0;}
	virtual bool createLeaf(
				const AosRundataPtr &rdata,
				const u64 parent_id,
				const AosBitmapObjPtr &bitmap){return true;}
	virtual bool insertBits(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 node_parent_id,
				const u64 node_id,
				const u64 bitmap_parent_id,
				const u64 bitmap_id){return true;}
	virtual bool removeBits(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 node_parent_id,
				const u64 node_id,
				const u64 bitmap_parent_id,
				const u64 bitmap_id){return true;}
	virtual bool rebuildBitmap(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 &node_id,
				const vector<u64> &parent_ids,
				const vector<u64> &member_ids){return true;}
	virtual bool removeBitmap(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 &node_id){return true;}
	virtual bool nodeLevelChanged(
				const AosRundataPtr &rdata, 
				const AosBitmapTreeObjPtr &tree,
				const u64 &node_id, 
				const int old_level,
				const int old_iillevel,
				const int new_level, 
				const int new_iillevel){return true;}
	virtual void setParent(const u64 &leaf_id,
					   const u64 &orig_parent){return;}
	virtual bool finish(
	 			const AosRundataPtr &rdata,
	 			const AosBmpExeCallerPtr &caller){return true;}
	virtual u64 getBitmapId1(
				const AosRundataPtr &rdata, 
				const u64 &iilid,
				const int iil_level,
				const int node_level){return 0;}
	virtual u64 getExecutorID(){return 0;}
	virtual void setExecutorID(const u64 &id){return;}
	virtual bool isDisable() const{return true;}
	virtual void disable(){return;}
	virtual void lock(){return;}
	virtual void unlock(){return;}	
	virtual bool isFull(){return true;}
};
#endif

