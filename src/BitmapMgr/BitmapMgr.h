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
#ifndef Aos_BitmapMgr_BitmapMgr_h
#define Aos_BitmapMgr_BitmapMgr_h

#include "AppIdGens/Ptrs.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/BitmapNode.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadPool.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosBitmapMgrSingleton,
						AosBitmapMgr,
						AosBitmapMgrSelf,
						OmnSingletonObjId::eBitmapMgr,
						"BitmapMgr");


class AosBitmapMgr : public AosBitmapMgrObj 
{
	OmnDefineRCObject;

	enum
	{
		eDftBitmapIdBlockSize 	= 100,
		eDftBitmapIdInitValue 	= 1000,
		eDftBitmapMaxId 		= 0x0fffffffffffffULL,	
		eDftBitmapidPrefix 		= 0x3ff0000000000000ULL,
		eMaxIdleRsltNode = 10000,

		eMaxRsltSize = 0x100,
		eMaxRsltByteSize = sizeof(AosBitmapNode*)*eMaxRsltSize,
		eBitmapSize = 0x100,
		eBitmapByteSize = sizeof(u64)*eBitmapSize,
		eSaveRsltByteSize = sizeof(u32)*eMaxRsltSize,

	};

private:
	queue<AosBitmapObjPtr> 	mBitmaps;
	OmnMutexPtr     				mLock;
	OmnCondVarPtr          			mCondVar;

	// Chen Ding, 2013/01/06
	AosSysIdGenU64Ptr	mBitmapIdGen;
	u64					mDftBitmapIdBlockSize;
	u64					mDftBitmapIdInitValue;
	u64					mDftBitmapMaxId; 
	u64					mDftBitmapidPrefix;

	OmnVList<AosBitmapNode*> 	mLeafNodeDirtyList;
	OmnVList<AosBitmapNode*> 	mLeafNodeCleanList;
	OmnVList<AosBitmapNode*> 	mBranchNodeList;
	int							mLeafNodeListSize;
	int							mBranchNodeListSize;
	OmnVList<AosBitmapObjPtr> 	mBitmapList;
	int							mBitmapListSize;

	OmnMutexPtr     			mBitmapLock;
	OmnMutexPtr     			mNodeLock;
	static OmnMutexPtr     		smPoolLock;
	static OmnThreadPoolPtr 	smThreadPool;
public:
	AosBitmapMgr();
	~AosBitmapMgr();

    static AosBitmapMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop(){return true;}
    virtual bool		config(const AosXmlTagPtr &def);

	virtual u64 nextBitmapId(
						const AosRundataPtr &rdata, 
						const u64 iilid, 
						const int node_level);

	virtual AosBitmapObjPtr getBitmap();
	virtual void returnBitmap(const AosBitmapObjPtr &bitmap);

	static void getNodes(AosBitmapNode** nodes,const u64 & num)
	{
		aos_assert(smPoolLock);
		if(num == 0)
		{
			return;
		}
		smPoolLock->lock();
		for(u64 i = 0;i < num;i++)
		{
			nodes[i] = (AosBitmapNode *)bitmap_node_pool::malloc();
		}
		smPoolLock->unlock();
	}
	
	static void returnNodes(AosBitmapNode **nodes, const u64 &num)
	{
		aos_assert(smPoolLock);
		if(num == 0)
		{
			return;
		}
		smPoolLock->lock();
		for(u64 i = 0;i < num;i++)
		{
			bitmap_node_pool::free(nodes[i]);
		}
		smPoolLock->unlock();
	}
	
/*	static AosBitmapNode* getNode(const u8 level)
	{
				
	//	AosBitmapNode * node2 = OmnNew AosBitmapNode(level);
		AosBitmapNode * node2 = (AosBitmapNode *)bitmap_node_pool::malloc();
//		node2->init(level);
		//AosBitmapNode * node2 = OmnNew AosBitmapNode(level);
		return node2;
	}
*/
/*	static void returnNode(AosBitmapNode *node)
	{
		if(node->mNodeLevel > 0)
		{
			node->clean();
		}
		bitmap_node_pool::free(node);
//		OmnDelete node;
		return;
	}
*/
	static u8 countU16Bit(const u16 number)
	{
		u16 num = number;
		int count = 0;
		for(int i = 0;i < 16;i ++)
		{
			count += (num & 1);
			num = num >> 1;
		}
		return count;
	}

	void getIDGen(const AosRundataPtr &rdata);
};
#endif
