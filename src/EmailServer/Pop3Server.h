////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_EmailServer_Pop3Server_h
#define AOS_EmailServer_Pop3Server_h 

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <python2.6/Python.h>
#include <map>

using namespace std;

class AosPop3Server : virtual public OmnRCObject
{
	OmnDefineRCObject;
	
	typedef map<OmnString, int>             UIDLMap;
	typedef map<OmnString, int>::iterator   UIDLMapItr;
	typedef map<OmnString, bool>            DELEMap;
	typedef map<OmnString, bool>::iterator  DELEMapItr;

private:
		OmnMutexPtr         		mLock;
		PyObject *                  mLoginFunc;
		PyObject *                  mRetrFunc;
		PyObject *                  mDeleFunc;
		PyObject *                  mGetLenFunc;
		PyObject *                  mGetUidlFunc;
		static PyObject *			mPop3ServerModule;
		UIDLMap						mUIDLMap;
		UIDLMapItr					mUIDLMapItr;
		DELEMap						mDeleMap;
		DELEMapItr					mDeleMapItr;
		OmnString 					mServer;

public:
	AosPop3Server(const AosXmlTagPtr &config);
	~AosPop3Server();

	bool 		login(
				const AosXmlTagPtr &data,
				const AosRundataPtr &rdata);
	bool 		dele(
				const AosXmlTagPtr &data,
				const AosRundataPtr &rdata);


private:
	
	bool 			deleForever(const AosRundataPtr &rdata);
	bool 			setDeleFlag(
								const OmnString &uidl,
								const AosRundataPtr &rdata);
	bool 	 		checkExist(
								const OmnString &key,
								const OmnString &server,
								const AosRundataPtr &rdata);
	void            svrInit(const AosXmlTagPtr &config);
	void            moduleInit(const AosXmlTagPtr &config);
	bool 			setXml(
					const OmnString &html,
					const OmnString &user,
					const OmnString &server,
					const int idx,
					OmnString &key,
					const AosRundataPtr &rdata);
	bool 			checkState(
					const OmnString &content,
					OmnString &state,
					const AosRundataPtr &rdata);
	OmnString       getServer(
					const OmnString &email_addr,
					const AosRundataPtr &rdata);
	bool			loginToPopSvr(
					const OmnString &user,
					const OmnString &passwd,
					const OmnString &server,
					const OmnString &emldir,
					const AosRundataPtr &rdata);
	int 			getEmlLen(
					const OmnString &server,
					const AosRundataPtr &rdata);
	OmnString 		getEmlUIDL(
					const int idx,
					const OmnString &user,
					PyObject * uFunc,
					const AosRundataPtr &rdata);
	OmnString 		retrEmail(
					const int idx,
					const OmnString &user,
					PyObject * rFunc,
					const AosRundataPtr &rdata);
};
#endif

