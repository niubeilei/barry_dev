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
// 01/30/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_VersionServer_VersionServer_h
#define AOS_VersionServer_VersionServer_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

OmnDefineSingletonClass(AosVersionServerSingleton,
		AosVersionServer,
		AosVersionServerSelf,
		OmnSingletonObjId::eVersionServer,
		"VersionServer");


class AosVersionServer : public AosVersionServerObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxVerLen = 20
	};

private:
	OmnString			mDirname;
	OmnString 			mFilename;
	OmnMutexPtr			mLock;

public:
	AosVersionServer();
	~AosVersionServer();

	// Singleton class interface
	static AosVersionServer *getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	bool		addVersionObj(
					const u64 &docid,
					const AosXmlTagPtr &olddoc,
					const AosXmlTagPtr &newdoc,
					const AosRundataPtr &rdata);
	bool 		addVersionObj(
					const AosXmlTagPtr &newdoc,
					const AosRundataPtr &rdata);
	u64		addDeletedVerObj(
					const AosXmlTagPtr &deldoc,
					const AosRundataPtr &rdata);
	AosXmlTagPtr getVersionObj(
					const OmnString &objid,
					const OmnString &version, 
					const AosRundataPtr &rdata);

	AosXmlTagPtr getVersionObj(
					const OmnString &objid,
					const OmnString &pctr_objid,
					const OmnString &version, 
					const AosRundataPtr &rdata);

	AosXmlTagPtr getVersionObj(
			const u64 &docid,
			const OmnString &objid,
			const OmnString &version, 
			const AosRundataPtr &rdata);

	AosXmlTagPtr getVersionObj(const u64 &docid, const AosRundataPtr &rdata);
	bool rebuildVersion();
	AosXmlTagPtr	getLastVersionObj(const u64 &docid, const AosRundataPtr &rdata);

	bool
	addVersionIdx(
		const u64 &docid,
		const u32 seqno, 
		const u32 offset, 
		const OmnString &version,
		const AosRundataPtr &rdata);

	bool	
	addVersionDoc(
		const u64 &docid,
		const OmnString &doc, 
		const OmnString &version,
		const AosRundataPtr &rdata);

private:
	OmnString 	incrementVersion(const OmnString &version);
	OmnString 	composeVersionIILKey(
					const OmnString &objid,
					const OmnString &version)
				{
					aos_assert_r(objid != "", "");
					aos_assert_r(version != "", "");
					OmnString key = objid;
					key << "_" << version;
					return key;
				}
};
#endif

