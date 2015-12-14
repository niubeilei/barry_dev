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
#ifndef Aos_QueryRslt_QueryRsltMgr_h
#define Aos_QueryRslt_QueryRsltMgr_h

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Porting/Sleep.h"
#include "QueryRslt/Ptrs.h"
//#include "QueryRslt/QueryRslt.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/ValList.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

OmnDefineSingletonClass(AosQueryRsltMgrSingleton,
						AosQueryRsltMgr,
						AosQueryRsltMgrSelf,
						OmnSingletonObjId::eQueryRsltMgr,
						"QueryRsltMgr");


class AosQueryRsltMgr : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
	};
	enum
	{
//		eMaxRsltNum = 2000000,
	};
private:

public:
	AosQueryRsltMgr();
	~AosQueryRsltMgr();

    static AosQueryRsltMgr*  	getSelf();
    virtual bool      	start(){return true;}
    virtual bool        stop(){return true;}
    virtual bool		config(const AosXmlTagPtr &def);

//	AosQueryNode* getTopNode()
//	{
//		AosQueryNode* node = getNode(AosQueryNode::eMaxLevel-1);
//		return node;
//	}
	

	// Chen Ding, 11/24/2012
	// Moved to QueryBitmapMgr
	// AosQueryBitmapObjPtr getBitmap()
	// {
	// 	mBitmapLock->lock();	
	// 	if(mBitmapListSize == 0)
	// 	{
	// 		mBitmapLock->unlock();	
	// 		return OmnNew AosQueryBitmap();
	// 	}
	// 	AosQueryBitmapObjPtr bitmap = mBitmapList.popFront();
	// 	mBitmapListSize  --;
	// 	mBitmapLock->unlock();	
	// 	return bitmap;
	// }

/*	AosQueryBitmapObjPtr getBitmap(AosQueryNode* node)
	{
		AosQueryBitmapObjPtr bitmap = getBitmap();
		if(node)
		{
			returnNode(bitmap->mNode);
			bitmap->mNode = node;
		}
		return bitmap;
	}
*/

	// Chen Ding, 11/24/2012
	// Moved to QueryBitmapMgr
	// void returnBitmap(const AosQueryBitmapObjPtr &bitmap)
	// {
	// 	bitmap->clean();
	// 	mBitmapLock->lock();
	// 	if(mBitmapListSize > eMaxIdleRsltNode)
	// 	{
	// 		mBitmapLock->unlock();
	// 		return;
	// 	}
	// 	mBitmapList.append(bitmap);
	// 	mBitmapListSize ++;
	// 	mBitmapLock->unlock();
	// }

/*	void returnCleanNode(AosQueryNode *node)
	{
		if(node->mNodeLevel > 0)
		{
			mNodeLock->lock();
			if(mBranchNodeListSize >= eMaxIdleRsltNode)
			{
				OmnDelete node;
				mNodeLock->unlock();
				return;
			}
			{
				mBranchNodeListSize ++;
				mBranchNodeList.append(node);
				mNodeLock->unlock();
				return;
			}
			// should never come here
			return;
		}
		
		// level 0 node
		mNodeLock->lock();
		if(mLeafNodeListSize >= eMaxIdleRsltNode)
		{
			OmnDelete node;
			mNodeLock->unlock();
			return;
		}
		mLeafNodeCleanList.append(node);
		mLeafNodeListSize ++;
		mNodeLock->unlock();
	}
*/

/*	void returnSingleNode(AosQueryNode *node)
	{
		mNodeLock->lock();
		if(mNodeListSize[node->mNodeLevel] >= eMaxIdleRsltNode)
		{
			mNodeLock->unlock();
			OmnDelete node;
			return;
		}
		mNodeDirtyList[node->mNodeLevel].append(node);
		mNodeListSize[node->mNodeLevel] ++;
		mNodeLock->unlock();
	}
*/	
};
#endif

