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
#ifndef Aos_SEInterfaces_BitmapStorageMgrObj_h
#define Aos_SEInterfaces_BitmapStorageMgrObj_h

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

class AosBitmapStorageMgrObj : public AosJimo
{
private:
	static AosBitmapStorageMgrObjPtr		smObject;

public:
	AosBitmapStorageMgrObj(const int version);

	static AosBitmapStorageMgrObjPtr getObject();

	// virtual bool saveBitmap(
	// 				const u64 &bitmap_id,
	// 				const AosBuffPtr &buff, 
	// 				const AosRundataPtr &rdata) = 0;

	// virtual AosBitmapObjPtr retrieveBitmap(
	// 				const u64 &bitmap_id,
	// 				const AosRundataPtr &rdata) = 0;

	// virtual bool retrieveIILBitmap(
	// 				const u64 &section_id,
	// 				AosBitmapObjPtr &bitmap, 
	// 				const AosBitmapObjPtr &partial_bitmap, 
	// 				const AosBitmapTreeObjPtr &node_list, 
	// 				const AosRundataPtr &rdata) = 0;
	
	virtual bool removeLevelOneNode(
					const AosRundataPtr &rdata, 
					const u64 &node_id) = 0;  

	virtual AosBuffPtr retrieveLevelOneNodeAsBuff(
					const AosRundataPtr &rdata, 
					const u64 node_id) = 0; 

	virtual bool returnDocids(const vector<u64> &docids) = 0;

	virtual AosBuffPtr retrieveBitmapBlock(
					const AosRundataPtr &rdata, 
					const u64 docid) = 0;

	virtual bool saveBlockIndex(
					const AosRundataPtr &rdata, 
					const u64 node_id,
					const AosBuffPtr &buff) = 0;

	virtual bool saveBitmapBlock(
					const AosRundataPtr &rdata, 
					const u64 docid,
					const AosBuffPtr &doc) = 0;

	virtual u64 nextBitmapDocid(const AosRundataPtr &rdata) = 0;

	virtual bool saveLevelOneNode(
					const AosRundataPtr &rdata, 
					const u64 &node_id,
					const AosBuffPtr &buff) = 0;

	virtual AosBmpBlockIndexObjPtr retrieveBlockIndex(
					const AosRundataPtr &rdata, 
					const u64 node_id, 
					const char *fname, 
					const int line) = 0;

	virtual bool removeNode(
					const AosRundataPtr &rdata,
					const u64 node_id) = 0; 

	virtual bool retrieveAllBitmapsFromNode(
					const AosRundataPtr &rdata, 
					const u64 node_id,
					const AosBitmapObjPtr &bitmap,
					const AosBitmapObjPtr &sec_filter) = 0;

	virtual bool retrieveAllBitmapsFromNode(
					const AosRundataPtr &rdata, 
					const u64 node_id,
					vector<AosBitmapObjPtr> &bitmaps,
					const AosBitmapObjPtr &sec_filter) = 0;

	virtual AosBitmapObjPtr retrieveLeafAsBitmap(
					const AosRundataPtr &rdata, 
					const u64 node_id, 
					const u64 parent_id) = 0;  
	virtual void			waitForTest() = 0;
	virtual void			signalForTest() = 0;
	virtual void			lockForTest() = 0;
	virtual AosBmpBlockIndexObjPtr retrieveBlockIndexNoLoad(
					const AosRundataPtr &rdata, 
					const u64 node_id, 
					bool &need_load,
					const char *fname, 
					const int line) = 0;
	virtual bool loadBlockIndex(
					const AosRundataPtr &rdata, 
					const u64 node_id,
					const AosBmpBlockIndexObjPtr &node) = 0; 
	virtual AosBuffPtr retrieveLeafAsBuff(
					const AosRundataPtr &rdata, 
					const u64 node_id, 
					const u64 parent_id) = 0; 
};

inline AosBitmapStorageMgrObjPtr AosGetBitmapStorageMgr() 
{
 	return AosBitmapStorageMgrObj::getObject();
}
#endif
