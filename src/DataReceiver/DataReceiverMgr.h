////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 11/18/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataReceiver_DataReceiverMgr_h
#define AOS_DataReceiver_DataReceiverMgr_h

#include "SEInterfaces/DataReceiverObj.h"
#include "SEInterfaces/Ptrs.h"

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"


OmnDefineSingletonClass(AosDataReceiverMgrSingleton,
						AosDataReceiverMgr,
						AosDataReceiverMgrSelf,
						OmnSingletonObjId::eDataReceiverMgr,
						"DataReceiverMgr");

class AosDataReceiverMgr
{
private:
	typedef map<OmnString, AosDataReceiverObjPtr>::iterator mItr_t;

	map<OmnString, AosDataReceiverObjPtr>					mDataReceivers;
	OmnMutexPtr												mLock;

public:
	AosDataReceiverMgr();
	~AosDataReceiverMgr();

    // Singleton class interface
	static AosDataReceiverMgr * getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	bool createDataReceiver(
			const OmnString &data_col_name,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);
	bool dataReceiver(
			const OmnString &collectorid,
			const AosBuffPtr &buff);

	bool finishDataReceiver(
			const OmnString &collectorid,
			const AosRundataPtr &rdata);
};
#endif
