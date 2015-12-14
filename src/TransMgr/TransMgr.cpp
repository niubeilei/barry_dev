////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TransMgr.cpp
// Description:
//	It maintains a list of all transactions for an SPNR.  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TransMgr/TransMgr.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Message/Resp.h"
#include "NMS/HouseKp.h"
#include "NMS/Nms.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "TransMgr/Trans.h"
#include "TransMgr/NullTrans.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
//#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(OmnTransMgrSingleton,
                 OmnTransMgr,
                 OmnTransMgrSelf,
                "OmnTransMgr");

OmnTransMgr::OmnTransMgr()
:
mLock(OmnNew OmnMutex()),
mCleanThreshold(eTransCleaningTimer)
{
}


bool
OmnTransMgr::start()
{
	//
	// Add itself to HouseKeeper to take care of cleaning aging
	// transactions.
	//
	OmnNmsSelf->addHouseKeepObj(this);

	return true;
}


bool
OmnTransMgr::stop()
{
	return true;
}


OmnTransMgr::~OmnTransMgr()
{
}


bool
OmnTransMgr::config(const AosXmlTagPtr &conf)
{
	//
	// It assumes:
	//	<TransMgrConfig>
	//	</TransMgrConfig>
	//

	if (!conf)
	{
		return true;
	}

	OmnRslt rslt(true);
	//AosXmlTagPtr configItem = conf->getFirstChild(OmnXmlItemName::eTransMgrConfig);
	//if (configItem.isNull())
	//{
	//	return true;
	//}
	return true;
}


bool
OmnTransMgr::addTrans(const OmnTransPtr &trans)
{
	OmnTrace << "To add transaction: " << (int)trans->getTransId() << endl;

	mLock->lock();
	mTransTable.add(trans);
	if (trans->isForeignTransIdValid())
	{
		// 
		// The transaction contains a valid foreign transaction ID. Add it 
		// to the mForeignTrans.
		//
		mForeignTrans.add(trans);
	}

	mLock->unlock();
	return true;
}


OmnTransPtr
OmnTransMgr::getTrans(const OmnRespPtr &resp, const bool removeFlag)
{
	//
	// It retrieves the transaction by the response message. This function
	// does not assume the transaction must be present. It is the caller
	// who will determine whether it is an error if the transaction is not found.
	//

	OmnTransPtr trans;
	mLock->lock();
	OmnTraceCP << "To get trans: " << (int)resp->getTransId() << endl;
	trans = mTransTable.get(trans, removeFlag);
	mLock->unlock();
	return trans;
}


OmnTransPtr
OmnTransMgr::getTrans(const int64_t &transId, const bool removeFlag)
{
	//
	// It retrieves the transaction by the response message. This function
	// does not assume the transaction must be present. It is the caller
	// who will determine whether it is an error if the transaction is not found.
	//
	static OmnNullTransPtr lsTrans = OmnNew OmnNullTrans();

	lsTrans->setTransId(transId);
	mLock->lock();
	OmnTraceCP << "To get trans: " << (int)transId << endl;
	OmnTransPtr trans = mTransTable.get(lsTrans.getPtr(), removeFlag);
	mLock->unlock();
	return trans;
}


void
OmnTransMgr::procHouseKeeping(const int64_t &tick)
{
    //
    // The housekeeping wants to check all the transactions in
    // the list to remove those too old ones.
    //

/*	OmnTraceHK << "Clean MgcpTrans" << endl;

	int totalCleaned = 0;
	int blockSize = OmnHouseKp::getBlockSize();
    mTransTable.resetItr(blockSize);

    long crtTick = OmnTimeSelf->getSecTick();
    int numBuckets;

    OmnValList<OmnAsciiHexHash<OmnMgcpTransCommPtr, eTableSize>::HashEntry>*
                buckets[blockSize];
	OmnMgcpTransCommPtr trans;
    while (mTransTable.hasMoreBuckets())
    {
		mLock->lock();
        mTransTable.getBuckets(buckets, numBuckets);
        for (int i=0; i<numBuckets; i++)
        {
            buckets[i]->reset();
            while (buckets[i]->hasMore())
            {
				trans = (buckets[i]->crtValue()).mValue;

                if (crtTick - trans->getTick() >= mCleanThreshold)
                {
                    //
                    // It is too old. Need to remove it
                    //
					OmnTraceHK << "Age out transaction: " 
						<< trans->getTransId() 
						<< ":" << trans.getRef() << endl;
					
					//
					// Ask the transaction to remove itself from its endpoint
					//
					trans->removeFromEndpoint();

					totalCleaned++;
                    buckets[i]->eraseCrt1();
                }
				else
				{
                	buckets[i]->next();
				}
            }
        }
		mLock->unlock();
    }
*/
	OmnTraceHK << "Finished Cleaning MgcpTrans" << endl;
}


OmnTransPtr			
OmnTransMgr::getTrans(const OmnIpAddr &addr, 
					  const int port,
					  const int64_t &transId, 
					  const bool removeFlag)
{
	OmnForeignTransId obj(addr, port, transId);
	return mForeignTrans.get(obj, removeFlag);
}
