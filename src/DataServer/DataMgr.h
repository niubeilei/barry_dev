////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DataMgr.h
// Description:
//	This class is designed for:
//	1. Synchronize data among redundant pair SPNRs
//  2. Populate networkwide data
//  3. Provide local data service to SPNR
//  4. Cache networkwide data
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_DataServer_DataMgr_h
#define Omn_DataServer_DataMgr_h

#include "Database/Ptrs.h"
#include "DataServer/Ptrs.h"
#include "Debug/Rslt.h"
#include "Obj/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(OmnDataMgrSingleton, 
						OmnDataMgr, 
						OmnDataMgrSelf,
						OmnSingletonObjId::eDataMgr, 
						"DataMgr");


class OmnDataMgr
{
	OmnDefineRCObject;

private:
	OmnString					mUserName;
	OmnString					mPassword;
	OmnString					mHostName;
	OmnString					mDbName;
	int							mDbPort;
	OmnString					mConfigFileName;
//	OmnDatabasePtr				mDatabase;

	//OmnMgcpEptRepliPtr			mEndpointReplicator;
	//OmnMgcpConnRepliPtr			mConnReplicator;

	OmnValList<OmnSysObjPtr>	mDataQueue;
	OmnMutexPtr					mQueueLock;
	OmnCondVarPtr				mQueueCondVar;

	int							mDbId;

public:
	OmnDataMgr();
	~OmnDataMgr();

	bool	start();
	bool	stop();
	OmnMgcpEndpointPtr 	getEndpointByPid(const OmnString &pid);
	OmnRslt	updateEndpointIpAddr(const OmnMgcpEndpointPtr &ep);
//	OmnDatabasePtr	getDatabase() const {return mDatabase;}

	int		getDbId() const {return mDbId;}

	OmnRslt	query(const OmnString &query, OmnDbRecordPtr &r);
	OmnRslt query(const OmnString &query, OmnRawTablePtr &raw);
	OmnRslt runSQL(const OmnString &query);

	bool	isTableTypeII(const OmnString &name) const;
	OmnRslt	updateDb(const OmnString &stmt, OmnString &errMsg);


	//
	// OmnThreadedObj interface
	//
	//virtual bool threadFunc(OmnThrdStatus::E &state, 
	//				const OmnThreadPtr &thread);
	//virtual bool signal(const int threadLogicId);
    //virtual void heartbeat();
    //virtual bool checkThread() const; 
    //virtual bool isCriticalThread() const; 


	virtual OmnString       getSysObjName() const {return "OmnDataMgr";}
	virtual OmnSingletonObjId::E  getSysObjId() const 
							{return OmnSingletonObjId::eDataMgr;}
	virtual OmnRslt         config(const OmnSysObjPtr &def);
	virtual OmnRslt         config(const OmnXmlParserPtr &def);

	// 
	// Chen Ding, 05/07/2003
	//
	int		shrinkTable(const OmnString &tablename, 
						const int percentage) const;
	bool	clearTable(const OmnString &tablename) const;

private:
};
#endif
