////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 01/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_QueryClient_QueryClient_h
#define AOS_QueryClient_QueryClient_h

#include "QueryClient/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/File.h"
#include "XmlUtil/Ptrs.h"

#include <map>
#include <list>

using namespace std;

OmnDefineSingletonClass(AosQueryClientSingleton,
						AosQueryClient,
						AosQueryClientSelf,
						OmnSingletonObjId::eQueryClient,
						"QueryClient");

class AosQueryClient : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxQueries = 5
	};

private:

	typedef map<OmnString, list<AosQueryReqObjPtr>::iterator> 			map_t;
	typedef map<OmnString, list<AosQueryReqObjPtr>::iterator>::iterator mapitr_t;

	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	u64					mQueryId;
	map_t				mQryReqsMap;
	list<AosQueryReqObjPtr> mQryReqs;
	
	// for query id 
	map<OmnString, u64>	mQueryIdMap;
	u64					mCurrQueryId;

public:
	AosQueryClient();
	~AosQueryClient();

    // OmnThreadedObj Interface
    virtual bool    	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    	signal(const int threadLogicId);
    virtual bool    	checkThread(OmnString &err, const int thrdLogicId) const;

    // Singleton class interface
    static AosQueryClient *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	AosQueryReqObjPtr	createQuery(
							const AosXmlTagPtr &def,
							const AosRundataPtr &rdata);
	bool				queryFinished(
							const AosQueryReqObjPtr &query,
							const AosRundataPtr &rdata);

	bool				executeQuery(
							const AosXmlTagPtr &def,
							AosDatasetObjPtr &dataset,
							const AosRundataPtr &rdata);

	bool				executeQuery(
							const AosXmlTagPtr &def,
							AosRecordsetObjPtr &recordset,
							const AosRundataPtr &rdata);

	bool				executeQuery(
							const AosXmlTagPtr &def,
							OmnString &contents,
							const AosRundataPtr &rdata);

	bool 				executeQuery(
							const AosXmlTagPtr &def,
							const AosDatasetObjPtr &inputDataset,
							AosDatasetObjPtr &dataset,
							const AosRundataPtr &rdata);
	u64					getQueryId(
							const OmnString &query,
							const OmnString &session_id,
							const AosRundataPtr &rdata);
};

#endif
