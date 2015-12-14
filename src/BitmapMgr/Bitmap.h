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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapMgr_Bitmap_h
#define Aos_BitmapMgr_Bitmap_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "SEUtil/SeTypes.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/BitmapNode.h"
#include "BitmapMgr/BitmapItr.h"
#include "SEInterfaces/BitmapObj.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/HashUtil.h"
#include "Util/ValList.h"
#include <boost/pool/singleton_pool.hpp>
#include <vector>
#include <hash_map>

using namespace std;
using boost::pool;
using boost::details::pool::guard;

#define INVALIDATEID (0xFFFFFFFFFFFFFFFFULL)// u64
#define AOS_MIN(a, b) (((a)<(b)) ? (a) : (b))
#define AOS_MAX(a, b) (((a)>(b)) ? (a) : (b))
struct pool_tag{}; 
typedef boost::singleton_pool<pool_tag, sizeof(AosBitmapNode)> bitmap_node_pool;

class AosBitmap : public AosBitmapObj
{	
	OmnDefineRCObject;
	enum Opr
	{
		eAnd,
		eOr,
		eInvalid
	};
	
	enum
	{
		eMaxCacheSize = 0x00ffffff
	};
	
	AosBitmapNode*		mNode;
	u64					mMask;
	u64					mFilter;
	AosBitmap::Opr		mOpr;
	AosBitmapItrPtr		mItr;
	u64					mSectionId;
	u64					mBitmapId;		// Chen Ding, 2013/02/05
	u64					mModifyId;		// Chen Ding, 2013/05/07
	int					mIILLevel;		// Chen Ding, 2013/04/19
	int					mNodeLevel;	// Chen Ding, 2013/02/10
	u64					mMemorySize;	// Chen Ding, 2013/02/11
	i64					mNumBits;	// Chen Ding, 2013/02/11

	u64					mCacheSize;
	u64					mCrtCachePos;
	AosBitmapNode**		mNodeCache;
	
	AosBuffPtr			mBuff;

	OmnMutexPtr			mLock;
	static u64		smMasks[8];		// Chen Ding, 2013/01/29, Moved from Node

public:
	AosBitmap();
	~AosBitmap();
	
	virtual bool checkDoc(const u64 docid);
	virtual bool checkDocSafe(const u64 docid)
	{
		mLock->lock();
		bool rslt = checkDoc(docid);
		mLock->unlock();
		return rslt;
	}
	
	virtual AosBitmapObjPtr createBitmap() const;
	virtual void	setOprAnd(){mOpr = eAnd;}
	virtual void	setOprOr(){mOpr = eOr;}
	virtual u64 getMemorySize();
	virtual bool toString();
	virtual OmnString dumpToStr(const int num_per_line);
	virtual void removeDocid(const u64 docid);
	virtual AosBitmapNode*	getNextLeafNode();
	virtual AosBitmapNode* 	getLeafNode(const u64 &filter);
	virtual void addLeafNode(AosBitmapNode* leaf_node);
	virtual void addLeafNodeCopy(AosBitmapNode* leaf_node);
	virtual bool isEmpty();
	virtual void appendDocid(const u64 docid);
	virtual void appendDocids(const u64* docs, const i64 &num_docs);
	virtual void appendDocidsToBuff(const u64* docs, const i64 &num_docs);

	virtual void saveToBuff(const AosBuffPtr &buff, const bool reset = true);
	virtual bool loadFromBuff(const AosBuffPtr &buff, const bool reset = true);

	virtual bool nextDocid(u64 &docid);
	virtual bool prevDocid(u64 &docid);
	virtual bool lastDocid(u64 &docid);

	bool	isOprAnd()const{return mOpr == eAnd;}
	bool	isOprOr()const{return mOpr == eOr;}


	static AosBitmapObjPtr countAndAnotB(
			    const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b); 

	static AosBitmapObjPtr countAnd(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	static AosBitmapObjPtr countOr(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	virtual AosBitmapObjPtr countAndStatic(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	virtual bool checkAndBitmap(
				const AosBitmapObjPtr &a,
	 			const AosBitmapObjPtr &b);
	virtual AosBitmapObjPtr countOrStatic(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	

	static AosBitmapObjPtr countAndBlock(
				const AosBitmapObjPtr* bitmaps,
				const u64 num_bitmaps);
	static AosBitmapObjPtr countOrBlock(
				const AosBitmapObjPtr* bitmaps,
				const u64 num_bitmaps);
				
    
    static AosBitmapObjPtr	countGroupOrAnd(
		      const AosBitmapObjPtr** bitmap_groups,
			  const u64* num_bitmaps,
			  const u64  num_groups);


	void mergeNode(AosBitmapNode* node_a,AosBitmapNode* node_b);

	virtual void clear()
	{		
		clean();
	}

	void clean();

	void reset()
	{
		expand();
		mItr = 0;		
	}

	virtual void resetBuff()
	{
		if(mBuff)
		{
			mBuff->reset();
		}
	}
	
	virtual u64 nextNDocid(u64 *list,int &count);
	virtual bool getDocids(vector<u64> &docids);
	virtual i64  getDocidCount();
	
	virtual void setSectionId(const u64 &sectionId){ mSectionId = sectionId;};
  			
	virtual bool getSections(const AosRundataPtr &rdata, map_t &bitmaps);
	virtual bool getSections(const AosRundataPtr &rdata, vector<u64> &sections);

	static bool compare(
				const AosBitmapObjPtr &b1, 
				const AosBitmapObjPtr &b2)
	{
		return b1->compareBitmap(b2);
	}
	
	virtual bool pack();	
	virtual bool expand();	
	virtual bool expand(const AosBuffPtr &buff);
	virtual bool isPacked();	
	virtual void packToBuff(const AosBuffPtr &buff);
	virtual AosBuffPtr getBuff()const{return mBuff;}
	virtual void setBuff(const AosBuffPtr &buff){mBuff = buff;}

	// Chen Ding, 2013/02/05
	virtual int getNodeLevel() const {return mNodeLevel;}
	virtual void setNodeLevel(const int level) {mNodeLevel = level;}
	virtual int getIILLevel() const {return mIILLevel;}
	virtual void setIILLevel(const int level) {mIILLevel = level;}
	virtual void setBitmapId(const u64 &bitmap_id) {mBitmapId = bitmap_id;} 
	virtual u64 getBitmapId() const {return mBitmapId;}
	virtual void setModifyId(const u64 &modify_id) {mModifyId = modify_id;}
	virtual void resetModifyId(){mModifyId = AosBitmapObj::eInvalidBitmapID;}

	virtual u64 getModifyId() const {return mModifyId;}
	virtual bool orBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool andBitmap(const AosBitmapObjPtr &bitmap);
	virtual void returnBitmap(const AosBitmapObjPtr &bitmap);
	virtual AosBitmapObjPtr createBitmap(
				const AosRundataPtr &rdata, 
				const AosBuffPtr &buff);
	virtual bool compareBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool includeBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool removeBits(const AosBitmapObjPtr &bitmap);

	virtual u32 getSectionId () const {return mSectionId;}

	virtual bool appendBitmap(const AosBitmapObjPtr &b);
	virtual AosBitmapObjPtr clone(const AosRundataPtr &rdata);
	virtual AosBitmapObjPtr cloneSafe(const AosRundataPtr &rdata)
	{
		mLock->lock();
		AosBitmapObjPtr rslt = clone(rdata);
		mLock->unlock();
		return rslt;
	}
	virtual bool printDocids();
	virtual i64  getNumBits();
	virtual void lock(){mLock->lock();}
	virtual void unlock(){mLock->unlock();}

	// Shawn, 2014/09/20
	bool splitBitmap(const int num, vector<AosBitmapObjPtr> &bitmaps);
	bool splitBitmapByValues(const vector<u64> &values,vector<AosBitmapObjPtr> &bitmaps);
	bool cloneFrom(const AosBitmapObjPtr &source);


	bool moveTo(const u64 &id, u64 &cur_docid, const bool reverse);
//	u64 getPage(const u64 page_size,const AosBitmapObjPtr &rslt_bmp,const bool reverse);
	bool getCrtDocid(u64 &docid);
	
	// Chen Ding, 2013/10/26
	static int getNumInstances();
	
	AosBitmapNode* getNode(const u8 level);
	
	void returnNode(AosBitmapNode *node)
	{
		if(node->mNodeLevel > 0)
		{
			node->clean(this);
		}
		returnSingleNode(node);
	}
	
	void returnSingleNode(AosBitmapNode *node);
	static void getTypeFilter(AosBuff* buff,u8 &type, u64 &filter);
	u64 getNumLeaf();
	static void exchangeContentSafe(
				const AosBitmapObjPtr &node_a, 
				const AosBitmapObjPtr &node_b)
	{
		if(node_a.getPtr() == node_b.getPtr())
		{
			return;
		}
		node_a->lock();
		node_b->lock();
		exchangeContent(
				node_a, 
				node_b);
		node_a->unlock();
		node_b->unlock();
	}
	
	static void exchangeContent(
				const AosBitmapObjPtr &node_a, 
				const AosBitmapObjPtr &node_b)
	{
		if(node_a.getPtr() == node_b.getPtr())
		{
			return;
		}
		AosBitmap *a = (AosBitmap*)node_a.getPtr();
		AosBitmap *b = (AosBitmap*)node_b.getPtr();
		AosBitmapNode* tmpNode = a->mNode;
		a->mNode = b->mNode;
		b->mNode = tmpNode;
		
		u64 tmpMask = a->mMask;
		a->mMask = b->mMask;
		b->mMask = tmpMask;
		
		u64 tmpFilter = a->mFilter;
		a->mFilter = b->mFilter;
		b->mFilter = tmpFilter;

		AosBitmap::Opr	tmpOpr = a->mOpr;
		a->mOpr = b->mOpr;
		b->mOpr = tmpOpr;

		AosBitmapItrPtr tmpItr = a->mItr;
		a->mItr = b->mItr;
		b->mItr = tmpItr;
		a->mNumBits = 0;
		b->mNumBits = 0;
		
		AosBuffPtr tmpBuff = a->mBuff;
		a->mBuff = b->mBuff;
		b->mBuff = tmpBuff;
	}
private:
	bool orBitmapPacked(const AosBitmapObjPtr &bitmap);
	bool andBitmapPacked(const AosBitmapObjPtr &bitmap);
	bool checkAndBitmapPacked(const AosBitmapObjPtr &a,
	 						  const AosBitmapObjPtr &b);
	static AosBitmapObjPtr countOrPacked(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	static AosBitmapObjPtr countAndPacked(
				const AosBitmapObjPtr &a,
				const AosBitmapObjPtr &b);
	virtual bool getSectionsPacked(const AosRundataPtr &rdata, map_t &bitmaps);
	void cleanPriv();


};
#endif

