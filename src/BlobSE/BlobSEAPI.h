////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 15 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_BlobSEAPI_H_
#define Aos_BlobSE_BlobSEAPI_H_
#include <deque>

#include "aosUtil/Types.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/Ptrs.h"
#include "UtilData/BlobSEReqEntry.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "JimoRaft/RaftStateMachine.h"

class AosBlobSEAPI : virtual public  AosRaftStateMachine
{
	OmnDefineRCObject;

private:
	typedef std::deque<AosBlobHeaderPtr> deq_header_t;

public:
	typedef AosBlobSE::Config Config;

private:
	OmnMutex*			mLock;
	AosBlobSEPtr		mBlobSE;
	deq_header_t		mHeaderDeque;
	u64					mLastAppliedOprID;
	u64					mLastAppendedOprID;
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mConf;

public:
    AosBlobSEAPI(
    		const AosRundataPtr	&rdata,
    		const AosXmlTagPtr	&conf);

    AosBlobSEAPI(
    		const AosRundataPtr	&rdata,
    		int					cubeId,
			const u64	  		aseId,
    		const u32			header_custom_data_size);

    virtual ~AosBlobSEAPI();

	virtual AosJimoPtr cloneJimo() const;  

	virtual OmnString getClassName()	{return "AosBlobSEAPI";}

    bool appendEntry(
			const AosRundataPtr	&rdata,
			const u32			ulTermID,
			const u64			ullOprID,
			const AosBuffPtr	&pBuff);

    bool getEntry(
			const AosRundataPtr	&rdata,
			const u64			ullOprID,
			u32					&ulTermID,
			AosBuffPtr			&pBuff);

    bool apply(
			const AosRundataPtr	&rdata,
			const u64			ullOprID);

	bool readDoc(
			const AosRundataPtr	&rdata,
			const u64			ullAseID,		//TODO:place holder
			const u64			ullSnapID,
			const u64 			ullDocid,
			const u64 			ullTimestamp,
			AosBuffPtr			&pBodyBuff,
			AosBuffPtr			&pHeaderCustomData);

	int doCompationAsRaftLeader();

	int shutdown(const AosRundataPtr	&rdata);

    const Config getConfig() const {return mBlobSE->getConfig();};

	static	bool genBlobSEData(
			AosBuffPtr &buff,                  
			const u64 snap_id,                 
			const u64 docid,                   
			const AosBlobSEReqEntry::ReqTypeAndResult reqType,
			const AosBuffPtr &docstr,
			AosBuffPtr pCustomeData = NULL);

	bool removeEntry(
			const AosRundataPtr	&rdata,
			const u64			ullOprID);

	bool init(
			const AosRundataPtr	&rdata,
			const u64			ullLastAppliedOprID);

	OmnString getBaseDir();

private:
    int saveHeaderToCache(
			const u64				ullOprID,
    		const AosBlobHeaderPtr&	pHeader);

    int getHeaderFromCacheByOprID(
			const u64			ullOprID,
    		AosBlobHeaderPtr&	pHeader);

    int recoverHeaders(const AosRundataPtr	&rdata);

    bool needsSuppressReadError(u64 eAseID);

};

#endif /* Aos_BlobSE_BlobSEAPI_H_ */
