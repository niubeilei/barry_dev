////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StoreMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataStore_StoreMgr_h
#define Omn_DataStore_StoreMgr_h

#include "DataStore/Ptrs.h"
#include "DataStore/StoreId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Mutex.h"
#include "Util/ValList.h"


OmnDefineSingletonClass(OmnStoreMgrSingleton,
                		OmnStoreMgr, 
						OmnStoreMgrSelf,
						OmnSingletonObjId::eStoreMgr, 
						"StoreMgr");


class OmnStoreMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutex					mLock;
	OmnVList<OmnDataStorePtr>	mStores;

	//
	// Do not use the two:
	//
	OmnStoreMgr(const OmnStoreMgr &rhs);
	OmnStoreMgr & operator = (const OmnStoreMgr &rhs);

public:
	OmnStoreMgr();
	virtual ~OmnStoreMgr();

	OmnDataStorePtr	getStore(const OmnStoreId::E storeId = OmnStoreId::eGeneral); 
	OmnRslt			addStore(const OmnDataStorePtr &store);
	bool			remoteStore(const OmnString &name);
	bool			storeExist(const OmnString &name);

	bool			query(const OmnString &query, OmnDbRecordPtr &r);
	bool			query(const OmnString &query, OmnDbTablePtr &raw);
	bool			runSQL(const OmnString &query);

	// 
	// Singleton class interface
	//
	static OmnStoreMgr *	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool 			config(const AosXmlTagPtr &def);
//	virtual OmnString       getSysObjName() const {return "OmnStoreMgr";}
//	virtual OmnSingletonObjId::E  getSysObjId() const {return OmnSingletonObjId::eStoreMgr;}
//	virtual OmnRslt         config(const OmnXmlParserPtr &def);
};
#endif

