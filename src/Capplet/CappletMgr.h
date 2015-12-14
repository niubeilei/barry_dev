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
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Capplet_CappletMgr_h
#define AOS_Capplet_CappletMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCOBjImp.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosCappletMgrSingleton,
						AosCappletMgr,
						AosCappletMgrSelf,
						OmnSingletonObjId::eCappletMgr,
						"CappletMgr");

class AosCappletMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str>::iterator mapitr_t;

private:
	OmnMutexPtr				mLock;
	map_t					mCappletMap;
	vector<AosCappletPtr>	mCapplets;

public:
	AosCappletMgr();
	~AosCappletMgr();

    // Singleton class interface
    static AosCappletMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
	bool registerCapplet(
				const AosRundataPtr &rdata, 
				const AosCappletPtr &capplet);

	bool runCapplet(
				const AosRundataPtr &rdata, 
				AosParms &parms, 
				const AosCappletId::E capplet_id, 
				const AosDataletPtr &data);

	bool runCapplet(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &parms,
				const AosCappletId::E capplet_id, 
				const AosDataletPtr &data);

private:
};

inline AosCappletMgr *AosGetCappletMgr() {return AosCappletMgr::getSelf();}
#endif
#endif
