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
// 09/04/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Snapshot_SnapShotInfo_h
#define AOS_Snapshot_SnapShotInfo_h

#include "Snapshot/SnapShot.h"
#include "Snapshot/Ptrs.h"
#include "Snapshot/SnapShotType.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"

class AosSnapShotInfo : virtual public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		eSnapShotKeyLen = sizeof(u32) + sizeof(u32) + sizeof(u32)
	};

	struct AosSnapShotKey
	{
	public:
		u32		virtual_id;
		u32		siteid;
		u32		dfm_id;

		AosSnapShotKey()
		:
		virtual_id(0),
		siteid(0),
		dfm_id(0)
		{
		}

		AosSnapShotKey(
				const u32 vid, 
				const u32 sid,
				const u32 did)
		:
		virtual_id(vid),
		siteid(sid),
		dfm_id(did)
		{
		}

		bool operator <(const AosSnapShotKey &rhs)const  
		{
			if (virtual_id == rhs.virtual_id)
			{
				if (siteid == rhs.siteid)
				{
					return dfm_id < rhs.dfm_id;
				}
				else
				{
					return siteid < rhs.siteid;
				}
			}
			return virtual_id < rhs.virtual_id;
		}

		void serializeTo(const AosBuffPtr &conf_buff)
		{
			conf_buff->setU32(virtual_id);
			conf_buff->setU32(siteid);
			conf_buff->setU32(dfm_id);
		}

		bool serializeFrom(const AosBuffPtr &conf_buff)
		{
			virtual_id = conf_buff->getU32(0);
			siteid = conf_buff->getU32(0);
			aos_assert_r(siteid, false);
			dfm_id = conf_buff->getU32(0);
			aos_assert_r(dfm_id, false);
			return true;
		}
	};


	typedef map<AosSnapShotKey, AosSnapShotPtr> map_t;
	typedef map<AosSnapShotKey, AosSnapShotPtr> ::iterator mapitr_t;

public:
	enum
	{
		eInfoMinSize = sizeof(u32) + sizeof(u8) + sizeof(u32) + AosSnapShotInfo::eSnapShotKeyLen + sizeof(u8)
	};

	AosSnapShotMgrPtr			mSnapShotMgr;
	u32 						mSnapSeqno;
	AosSnapShotType::E			mType;
	map_t						mSnapShot;
	AosSnapShot::Status			mSts;
	bool						mShowLog;

	AosSnapShotInfo(
			const AosSnapShotMgrPtr &snap_shot_mgr,
			const u32 id, 
			const AosSnapShotType::E type,
			const bool show_log);

	~AosSnapShotInfo();

	void init(const AosRundataPtr &rdata, const bool create);	

	bool clearData();

	AosSnapShotPtr  serializeFromMapEntry(
			AosSnapShotKey &key,
			const AosBuffPtr &conf_buff);

	bool serializeFrom(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &conf_buff);

	bool serializeTo(const AosBuffPtr &conf_buff);

	void readInfoFromFile(const AosRundataPtr &rdata);

	void saveInfoToFile();

	AosSnapShotPtr	retrieveSnapShot(
			const AosRundataPtr &rdata,
			const u32 virtual_id,
			const u32 dfm_id,
			const bool create_flag);

	AosSnapShotPtr newSnapShotObj(
			const u32 virtual_id,
			const u32 dfm_id,
			const AosSnapShot::Status sts);

	bool updateInfo(
			const AosRundataPtr &rdata,
			const AosSnapShot::Status sts,
			const u32 virtual_id,
			const u32 dfm_id);

	void updateStatus(const AosSnapShot::Status sts);
};
#endif
