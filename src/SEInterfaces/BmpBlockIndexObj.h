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
#ifndef Aos_SEInterfaces_BmpBlockIndexObj_h
#define Aos_SEInterfaces_BmpBlockIndexObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosBmpBlockIndexObj : virtual public OmnRCObject
{
private:
	static AosBmpBlockIndexObjPtr		smObject;

public:
	virtual AosBmpBlockIndexObjPtr create(
						const AosRundataPtr &rdata, 
						const u64 node_id, 
						const int node_level,
						const int iil_level, 
						const char *fname, 
						const int line) = 0;

	virtual bool collectDocidsSafe(
					const AosRundataPtr &rdata, 
					vector<u64> &docids) = 0;

	virtual u64 getDocidByLocalSectionIdSafe(const AosRundataPtr &rdata, const u64 &local_sid) = 0;

	virtual AosBitmapObjPtr getSectionedBitmapSafe(
					const AosRundataPtr &rdata, 
					const u32 global_sid) = 0;

	virtual bool readAllBitmapsSafe(
					const AosRundataPtr &rdata, 
					const AosBitmapObjPtr &bitmap, 
					const AosBitmapObjPtr &sec_filter) = 0;

	virtual bool readAllBitmapsSafe(
					const AosRundataPtr &rdata, 
					AosBitmapObj::map_t &sections,
					const AosBitmapObjPtr &sec_filter) = 0;

	virtual bool saveNodeSafe(const AosRundataPtr &rdata) = 0;

	virtual bool insertBitsSafe(const AosRundataPtr &rdata, 
					const vector<AosBitmapObjPtr> &bits) = 0;

	virtual bool removeBitsSafe(const AosRundataPtr &rdata, 
					const vector<AosBitmapObjPtr> &sectioned_bitmaps) = 0;

	virtual u64 getNodeId() const = 0;

	virtual bool rebuildNodeSafe(
					const AosRundataPtr &rdata, 
					const AosBitmapExecutorSvrObjPtr &executor,
					const u64 node_id,
					const int iil_level,
					const int node_level,
					const vector<u64> &member_ids) = 0; 

	virtual bool serializeFromBuff(const AosRundataPtr &rdata, const AosBuffPtr &buff) = 0;
	virtual void setSiteid(const u32 &id) = 0;
	virtual u32 getSiteid() const  = 0;
	virtual void setPrev(const AosBmpBlockIndexObjPtr &p)  = 0;
	virtual void setNext(const AosBmpBlockIndexObjPtr &n)  = 0;
	virtual AosBmpBlockIndexObjPtr getPrev() const = 0;
	virtual AosBmpBlockIndexObjPtr getNext() const = 0;
	virtual void resetLinkNode() = 0;
	virtual bool lock() = 0;
	virtual bool unlock() = 0;
};
#endif
