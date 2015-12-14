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
// 2013/02/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_QueryRslt_BitmapTest_h
#define Aos_QueryRslt_BitmapTest_h

#include "SEInterfaces/BitmapObj.h"
#include "Util/RCObject.h"
#include "Util/BuffArray.h"
#include "Util/HashUtil.h"
#include "Util/ValList.h"
#include <vector>
#include <set>
using namespace std;


class AosBitmapTest : public AosBitmapObj
{	
	OmnDefineRCObject;

private:
	u64					mBitmapId;
	u64					mSectionId;
	int					mNodeLevel;
	int					mIILLevel;
	set<u64>			mData;
	set<u64>::iterator 	mItr;
	
public:
	AosBitmapTest();
	~AosBitmapTest();
	
	virtual bool checkDoc(const u64 docid)const;
	virtual AosBitmapObjPtr getBitmap();
	virtual void	setOprAnd(){}
	virtual void	setOprOr(){}
	virtual u64 getMemorySize();
	virtual bool toString() {return false;}
	virtual void removeDocid(const u64 docid);
	virtual AosQueryNode*	getNextLeafNode() {return 0;}
	virtual AosQueryNode* 	getLeafNode(const u64 &filter) const {return 0;}
	virtual void addLeafNode(AosQueryNode* leaf_node) {}
	virtual bool isEmpty();
	virtual void appendDocid(const u64 docid);
	virtual void appendBlock(const u64* docs, const int num_docs);
	virtual void saveToBuff(const AosBuffPtr &buff);
	virtual bool loadFromBuff(const AosBuffPtr &buff);
	virtual u64 nextDocid();
	virtual void setCtrlBitmapNoCtrl() {}
	virtual bool isCtrlBitmapNoCtrl() {return false;};
	virtual void setCtrlBitmapNeedCtrl() {}
	virtual bool isCtrlBitmapNeedCtrl() {return false;}
	virtual void setCtrlBitmapIsCtrl() {}
	virtual bool isCtrlBitmapIsCtrl() {return false;};

	AosBitmapObjPtr getCtrlBitmap() const
	{
		return 0;
	}

	virtual void clear()
	{		
		clean();
	}

	void clean();

	void reset()
	{
		mItr = mData.begin();
	}

	virtual u64 nextNDocid(u64 *list,int &count);
	virtual bool getDocids(vector<u64> &docids);
	virtual u64  getDocidCount();
	virtual void setSectionId(const u64 &sectionId){ mSectionId = sectionId;}
	virtual u64  getSectionId() const {return mSectionId;}
	virtual bool getSections(
	    	const AosRundataPtr &rdata,
	        map_t &bitmaps);

	// Chen Ding, 2013/02/05
	virtual int getNodeLevel() const {return mNodeLevel;}
	virtual void setNodeLevel(const int level) {mNodeLevel = level;}
	virtual int getIILLevel() const {return mIILLevel;}
	virtual void setIILLevel(const int level) {mIILLevel = level;}
	virtual void setBitmapId(const u64 &bitmap_id) {mBitmapId = bitmap_id;}
	virtual u64 getBitmapId() const {return mBitmapId;}
	virtual bool orBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool andBitmap(const AosBitmapObjPtr &bitmap);
	virtual AosBitmapObjPtr getBitmap() const
	{
		return OmnNew AosBitmapTest();
	}
	virtual void returnBitmap(const AosBitmapObjPtr &bitmap);
	virtual AosBitmapObjPtr createBitmap(
				const AosRundataPtr &rdata, 
				const AosBuffPtr &buff);
	virtual bool compareBitmap(const AosBitmapObjPtr &bitmap);
	virtual bool removeBits(const AosBitmapObjPtr &bitmap);
	virtual bool splitByPhysical(
						vector<AosBitmapObjPtr> &bitmaps,
						const AosRundataPtr &rdata);
	virtual AosBitmapObjPtr countAndStatic(const AosBitmapObjPtr &a,
										  const AosBitmapObjPtr &b){return 0;}

	virtual AosBitmapObjPtr countOrStatic(const AosBitmapObjPtr &a,
										  const AosBitmapObjPtr &b){return 0;}
	virtual bool appendBitmap(const AosBitmapObjPtr &b){return false;}
	
};
#endif

