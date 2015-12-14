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
#ifndef AOS_IILUtil_IILSave_h
#define AOS_IILUtil_IILSave_h

#include "alarm_c/alarm.h"
#include "DfmUtil/Ptrs.h"
#include "IILUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/TransId.h"
#include "AppMgr/App.h"
#include "JimoAPI/JimoDocStore.h"

#include <map>

OmnDefineSingletonClass(AosIILSaveSingleton,
						AosIILSave,
						AosIILSaveSelf,
						OmnSingletonObjId::eIILSave,
						"IILSave");


class AosIILSave : public OmnRCObject 
{
	OmnDefineRCObject;

	typedef map<u64, AosIILDfmDocsPtr> map_t;
	typedef map<u64, AosIILDfmDocsPtr> ::iterator mapitr_t;

private:
	map_t					mIILTransMaps;
	OmnMutexPtr				mLock;
	bool					mShowLog;
	u64						mCrtSnapId;
	map<u64, u64>			mSnapIds;

public:

	AosIILSave();
	~AosIILSave();

public:
    // Singleton class interface
    static AosIILSave* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool add(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			u64 &snap_id);

	bool saveDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &doc_mgr,
			vector<AosTransId> &trans,
			const AosDfmDocPtr &doc,
			const u64 &snap_id);

	AosDfmDocPtr readDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &doc_mgr,
			const u64 snap_id,
			const u64 local_iilid,
			const bool read_body);

	bool deleteDoc(
			const AosRundataPtr &rdata,
			const AosDocFileMgrObjPtr &doc_mgr,
			vector<AosTransId> &trans_ids,
			const AosDfmDocPtr &doc,
			const u64 snap_id);

	bool commit(
			const AosRundataPtr &rdata,
			const u64 root_iilid,
			const AosDocFileMgrObjPtr &doc_mgr,
			const u64 snap_id);

	bool	isSmallIILTrans(const u64 &snap_id);
private:
	void	addEntryToMapsPriv(const u64 &iilid, u64 &snap_id); 
	void	removeEntryFromMapsPriv(const u64 &iilid, const u64 &snap_id);
	AosIILDfmDocsPtr getEntryFromMapsPriv(const u64 &root_iilid);
	u64		isExist(const u64 &root_iilid);
};

#endif

