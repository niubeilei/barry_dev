////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	
// Modification History:
// 04/08/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmConfig_h
#define AOS_DfmUtil_DfmConfig_h

#include "DfmUtil/DfmDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/Buff.h"

#define AOS_DFM_BODYENGINE_SLAB		"slab"
#define AOS_DFM_BODYENGINE_LOG		"log"

struct AosDfmConfig 
{
	AosDfmDocType::E	mDocType;
	OmnString			mCompressAlg;
	AosModuleId::E		mModuleId;
	OmnString 			mPrefix;			// Used to prefix files

	bool				mNeedLog;			// Not used in 3.x
	u32					mSnapShotNum;		// Not used in 3.x
	bool				mShowLog;			// Not used in 3.x
	bool				mNeedSnapshotMgr;	// Not used in 3.x
	int					mHeaderSize;		// Gavin, 2015/10/13

	AosDfmConfig(){};
	AosDfmConfig(const AosDfmDocType::E doc_type,
			const OmnString &prefix = "Data",
			const AosModuleId::E m_id = AosModuleId::eInvalid,
			const bool need_log = true,
			const u32 snap_num = 1,
			const OmnString &compress_alg = "gzip",
			const bool	show_log = false, 
			const int header_size = -1)
	:
	mDocType(doc_type),
	mCompressAlg(compress_alg),
	mModuleId(m_id),
	mPrefix(prefix),
	mNeedLog(need_log),
	mSnapShotNum(snap_num),
	mShowLog(show_log),
	mNeedSnapshotMgr(true),
	mHeaderSize(header_size)		// Gavin, 2015/10/13
	{
		mHeaderSize = AosDfmDoc::getHeaderSize(doc_type);
	}
	
	AosDfmConfig(const AosDfmDocType::E doc_type,
			const OmnString &prefix,
			const AosModuleId::E module_id,
			const OmnString &compress_alg = "gzip", 
			const int header_size = -1)
	:
	mDocType(doc_type),
	mCompressAlg(compress_alg),
	mModuleId(module_id),
	mPrefix(prefix),
	mNeedLog(false),
	mSnapShotNum(1),
	mShowLog(false),
	mNeedSnapshotMgr(true),
	mHeaderSize(header_size)		// Gavin, 2015/10/13
	{
		mHeaderSize = AosDfmDoc::getHeaderSize(doc_type);
	}
	
	bool serializeTo(const AosBuffPtr &dfm_buff) const
	{
		aos_assert_r(dfm_buff, false);	
		
		dfm_buff->setU8(mDocType);
		dfm_buff->setOmnStr(mPrefix);
		dfm_buff->setU8(mModuleId);
		dfm_buff->setU8(mNeedLog);
		dfm_buff->setU32(mSnapShotNum);
		dfm_buff->setOmnStr(mCompressAlg);
		dfm_buff->setU8(mShowLog);
		dfm_buff->setU8(mNeedSnapshotMgr);
		dfm_buff->setInt(mHeaderSize);

		return true;
	}

	static bool serializeFrom(AosDfmConfig & dfm_conf, const AosBuffPtr &dfm_buff)
	{
		aos_assert_r(dfm_buff, false);	
		
		dfm_conf.mDocType = (AosDfmDocType::E)dfm_buff->getU8(0);
		dfm_conf.mPrefix = dfm_buff->getOmnStr("");
		dfm_conf.mModuleId = (AosModuleId::E)dfm_buff->getU8(0);
		dfm_conf.mNeedLog = dfm_buff->getU8(0);
		dfm_conf.mSnapShotNum = dfm_buff->getU32(0);
		dfm_conf.mCompressAlg = dfm_buff->getOmnStr("");
		dfm_conf.mShowLog = dfm_buff->getU8(0);
		dfm_conf.mNeedSnapshotMgr = dfm_buff->getU8(0);
		dfm_conf.mHeaderSize = dfm_buff->getInt(-1);
		
		return true;
	}
	
	bool isSame(const AosDfmConfig &conf)
	{
		if(mDocType != conf.mDocType)	return false;	
		if(mModuleId != conf.mModuleId)	return false;	
		return true;
	}
	
	int getHeaderSize() const {return mHeaderSize;}		// Gavin, 2015/10/13
};

#endif
