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
// 11/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_Bitmap_h
#define Aos_SEInterfaces_Bitmap_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"

class AosBitmapNode;

class AosBitmapObj : public OmnRCObject
{
	static AosBitmapObjPtr	smObject;

protected:
	bool		mIsDirty;

public:
	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp> map_t;
	typedef hash_map<const u64, AosBitmapObjPtr, u64_hash, u64_cmp>::iterator mapitr_t;

	enum
	{
		eDftMaxSectionId = 0xffff,
		eInvalidBitmapID = 0,
		eNewLeaf = 0xffffffffffffffffULL
	};

public:
	AosBitmapObj()
	:
	mIsDirty(false)
	{
	}

	virtual ~AosBitmapObj();

	virtual bool checkDoc(const u64 docid) = 0;
	virtual bool checkDocSafe(const u64 docid) = 0;
	virtual AosBitmapObjPtr clone(const AosRundataPtr &rdata) = 0;
	virtual AosBitmapObjPtr cloneSafe(const AosRundataPtr &rdata) = 0;
	virtual u64 getBitmapId() const = 0;
	virtual u64 getMemorySize() = 0;
	virtual void setBitmapId(const u64 &bitmap_id) = 0;
	virtual void setNodeLevel(const int level) = 0;
	virtual int getNodeLevel() const = 0;
	virtual void setIILLevel(const int level) = 0;
	virtual int getIILLevel() const = 0;
	virtual bool orBitmap(const AosBitmapObjPtr &bitmap) = 0;
	virtual bool andBitmap(const AosBitmapObjPtr &bitmap) = 0;
	virtual u64 getModifyId() const = 0;
	virtual bool getSections(const AosRundataPtr &rdata, map_t &bitmaps) = 0;
	virtual bool getSections(const AosRundataPtr &rdata, vector<u64> &sections) = 0;
	virtual bool removeBits(const AosBitmapObjPtr &bitmap) = 0;
	virtual void saveToBuff(const AosBuffPtr &buff, const bool reset = true) = 0;
	virtual bool loadFromBuff(const AosBuffPtr &buff, const bool reset = true) = 0;
	virtual u32 getSectionId () const = 0;
	void setDirtyFlag(const bool f) {mIsDirty = f;}
	virtual i64  getNumBits() = 0;
	virtual void reset() = 0;
	virtual bool nextDocid(u64 &docid) = 0;
	virtual bool lastDocid(u64 &docid) = 0;
	virtual bool prevDocid(u64 &docid) = 0;
	bool isDirty() const {return mIsDirty;}
	virtual void setModifyId(const u64 &modify_id) = 0;			
	virtual void resetModifyId() = 0;
	virtual bool isEmpty() = 0;
	virtual OmnString dumpToStr(const int num_per_line) = 0;
	virtual void appendDocids(const u64* docs, const i64 &num_docs) = 0;
	virtual void appendDocid(const u64 docid) = 0;
	virtual void clean() = 0;
	virtual bool compareBitmap(const AosBitmapObjPtr &bitmap) = 0;
	virtual void setSectionId (const u64 &section_id ) = 0;
	virtual bool getDocids(vector<u64> &docids) = 0;
	virtual bool includeBitmap(const AosBitmapObjPtr &bitmap) = 0;
	virtual bool toString() = 0;
	virtual void clear() = 0;
	virtual void setOprOr() = 0;
	virtual void removeDocid(const u64 docid) = 0;
	virtual AosBitmapObjPtr createBitmap() const = 0;
	virtual AosBitmapNode*   getNextLeafNode() = 0;
	virtual AosBitmapNode*   getLeafNode(const u64 &filter) = 0;
	virtual void addLeafNode(AosBitmapNode* leaf_node) = 0;
	virtual void addLeafNodeCopy(AosBitmapNode* leaf_node) = 0;
	virtual AosBitmapObjPtr countAndStatic(const AosBitmapObjPtr &a,
	 									  const AosBitmapObjPtr &b) = 0;
	virtual bool checkAndBitmap(const AosBitmapObjPtr &a,
	 							const AosBitmapObjPtr &b) = 0;
	virtual AosBitmapObjPtr createBitmap(
				const AosRundataPtr &rdata, 
				const AosBuffPtr &buff) = 0;
	
	virtual bool splitBitmap(const int num, vector<AosBitmapObjPtr> &bitmaps) = 0;
	virtual bool splitBitmapByValues(const vector<u64> &values,vector<AosBitmapObjPtr> &bitmaps) = 0;
	virtual bool cloneFrom(const AosBitmapObjPtr &source) = 0;
	virtual bool moveTo(const u64 &id, u64 &cur_docid, const bool reverse) = 0;
//aaa	virtual u64 getPage(const u64 page_size,const AosBitmapObjPtr &rslt,const bool reverse) = 0;
	virtual bool getCrtDocid(u64 &docid) = 0;
	//virtual void setOprAnd() = 0;
	//virtual bool appendBitmap(const AosBitmapObjPtr &b) = 0;
	//virtual bool printDocids() = 0;

	virtual bool pack() = 0;	
	virtual bool expand() = 0;	
	virtual bool isPacked() = 0;	
	virtual AosBuffPtr getBuff() const = 0;
	virtual void resetBuff() = 0;
	virtual void setBuff(const AosBuffPtr &buff) = 0;
	
	// virtual AosBitmapObjPtr countOrStatic(const AosBitmapObjPtr &a,
	//									  const AosBitmapObjPtr &b) = 0;
	
	static AosBitmapObjPtr createBitmapStatic();
	static bool setObject(const AosBitmapObjPtr &obj);
	static AosBitmapObjPtr getObject();

	virtual AosBitmapNode* getNode(const u8 level) = 0;
	virtual void returnNode(AosBitmapNode *node) = 0;
	static inline u32 docid2SectionId(const u64 &docid)
	{
		// Bitmap Section ID contains the following bits:
		// 00111100 00111100 00111111 11000000 00000000
		// 					   0x3fc000 (Bit 21-14, 8 bits)
		// 		      0x3c000000 (Bit 29-26, 4 bits)
		//   0x3c00000000 (Bit 37-34, 4 bits)
		// return ((u32)(((docid&0x0000003c00000000ULL)>>26) 
		// 			| ((docid&0x000000003c000000ULL)>>22) 
		// 			| ((docid&0x00000000003fc000ULL)>>14)));	

		// Chen Ding, 2013/10/14
		// Sections are identified by Bits 32-63
		// return (u32)(docid >> 32);
		//
		// Chen Ding, 2013/10/17
		// Using Bits 11111000 00000000 00000000
		u32 sid1 = (u32)((docid >> 32 & 0x00ffffff)) << 5;
		u32 sid2 = ((u32)(docid & 0x00f80000ULL)) >> 19;
		return sid1 + sid2;
	}
	
	virtual void lock() = 0;
	virtual void unlock() = 0;
	

	static u32 getDftMaxSectionId() {return eDftMaxSectionId;}
};
#endif



