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
#ifndef Aos_SEInterfaces_IILExecutorObj_h
#define Aos_SEInterfaces_IILExecutorObj_h

#include "Rundata/Ptrs.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include <vector>
using namespace std;


class AosIILExecutorObj : virtual public AosRundataParm 
{
	static AosIILExecutorObjPtr	smObject;
	
public:
	AosIILExecutorObj();
	static void setObject(const AosIILExecutorObjPtr &obj) {smObject = obj; }
	static AosIILExecutorObjPtr getObject() {return smObject;}

	virtual u64 createTempBitmap(
				const AosRundataPtr &rdata,
				const AosBitmapObjPtr &bitmap) = 0;
	virtual bool createLeaf(
				const AosRundataPtr &rdata,
				const u64 parent_id,
				const AosBitmapObjPtr &bitmap) = 0;
	virtual bool insertBits(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 node_parent_id,
				const u64 node_id,
				const u64 bitmap_parent_id,
				const u64 bitmap_id) = 0;
	virtual bool removeBits(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 node_parent_id,
				const u64 node_id,
				const u64 bitmap_parent_id,
				const u64 bitmap_id) = 0;
	virtual bool rebuildBitmap(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 &node_id,
				const vector<u64> &parent_ids,
				const vector<u64> &member_ids) = 0;
	virtual bool removeBitmap(
				const AosRundataPtr &rdata,
				const int iil_level,
				const int node_level,
				const u64 &parent_id,
				const u64 &node_id) = 0; 
	virtual bool nodeLevelChanged(
				const AosRundataPtr &rdata, 
				const AosBitmapTreeObjPtr &tree,
				const u64 &node_id, 
				const int old_level,
				const int old_iillevel,
				const int new_level, 
				const int new_iillevel) = 0;
	virtual void setParent(const u64 &leaf_id,
					   const u64 &orig_parent) = 0;
	virtual bool finish(
	 			const AosRundataPtr &rdata,
	 			const AosBmpExeCallerPtr &caller) = 0;
	virtual u64 getBitmapId1(
				const AosRundataPtr &rdata, 
				const u64 &iilid,
				const int iil_level,
				const int node_level) = 0;
	virtual u64 getExecutorID() const = 0;
	virtual void setExecutorID(const u64 &id) = 0;
	virtual bool isDisable() const = 0;
	virtual void disable() = 0;
	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual bool isFull() = 0;
};
#endif

