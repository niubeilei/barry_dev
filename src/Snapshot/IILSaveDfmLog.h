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
// 	Created: 09/06/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Snapshot_IILSaveDfmLog_h
#define AOS_Snapshot_IILSaveDfmLog_h

#include "alarm_c/alarm.h"
#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Snapshot/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Snapshot/SnapShotMgr.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/TransId.h"

#include <map>

OmnDefineSingletonClass(AosIILSaveDfmLogSingleton,
						AosIILSaveDfmLog,
						AosIILSaveDfmLogSelf,
						OmnSingletonObjId::eIILSaveDfmLog,
						"IILSaveDfmLog");

class AosIILSaveDfmLog : public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		eEntrySize = AosSnapShotMgr::eEntryMinSize
	};

	typedef map<u64, u64> map_t;
	typedef map<u64, u64> ::iterator mapitr_t;

private:
	AosSnapShotMgrPtr		mSnapShotMgr;
	map_t					mIILMaps;
	OmnMutexPtr				mLock;
	bool					mShowLog;

public:

	AosIILSaveDfmLog();
	~AosIILSaveDfmLog();

public:
    // Singleton class interface
    static AosIILSaveDfmLog* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool addSnapShot(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			u64 &snap_id);

	bool saveDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &docfilemgr,
			vector<AosTransId> &trans,
			const AosDfmDocPtr &doc,
			const u64 snap_id);

	AosDfmDocPtr readDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &docfilemgr,
			const u64 snap_id,
			const u64 local_iilid,
			const bool read_body);

	bool deleteDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &docfilemgr,
			vector<AosTransId> &trans_ids,
			const AosDfmDocPtr &doc,
			const u64 snap_id);

	bool removeSnapShot(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			const u64 snap_id,
			const u32 virtual_id,
			const u32 dfm_id);

	bool rollBack(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			const u64 snap_id,
			const u32 virtual_id,
			const u32 dfm_id);

	bool commit(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			const AosDocFileMgrObjPtr &docFileMgr,
			const u64 snap_id);

	bool rollBackCb(
			const AosRundataPtr &rdata,
			const u64 snap_id,
			const u32 virtual_id,
			const u32 dfm_id);

	u64 	getSnapIdByIILID(const u64 iilid);
private:
	void	addEntryToMaps(const u64 iilid, const u64 snap_id); 
	void	removeEntryFromMaps(const u64 iilid, const u64 snap_id);
};

#endif

