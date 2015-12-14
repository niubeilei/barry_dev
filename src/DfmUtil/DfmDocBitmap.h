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
// 2013/02/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmDocBitmap_h
#define AOS_DfmUtil_DfmDocBitmap_h

// #include "DocFileMgr/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

class AosDfmDocBitmap : public AosDfmDoc 
{
public:
	enum
	{
		eBitmapIdOff = sizeof(u32) + sizeof(u64),
		eHeaderSize = 50
	};

private:
	u64				mBitmapId;
	int				mNodeLevel;
	int				mIILLevel;

	static bool		smShowLog;

public:
	AosDfmDocBitmap(const bool regflag);
	AosDfmDocBitmap(const u64 &docid, const int node_level = -1, const int iil_level = -1);
	~AosDfmDocBitmap();

	// AosDfmDoc Interface.
	virtual AosDfmDocPtr clone(const u64 docid);
	virtual u32 getHeaderSize(){ return eHeaderSize; };

	virtual bool serializeFrom(
				const AosBuffPtr &header_buff,
				u32 &body_seq,
				u64 &body_off);
	virtual bool serializeTo(
				const AosBuffPtr &header_buff,
				const u32 body_seq,
				const u64 body_off);
	virtual bool getBodySeqOffFromBuff(
				const AosBuffPtr &header_buff,
				u32 &body_seq,
				u64 &body_off);
	virtual bool setBodySeqOffToBuff(
				const AosBuffPtr &header_buff,
				const u32 body_seq,
				const u64 body_off);
	// AosDfmDoc Interface End.

	void setBitmapId(const u64 &bitmap_id) {mBitmapId = bitmap_id;}
	void setNodeLevel(const int level) {mNodeLevel = level;}
	void setIILLevel(const int level) {mIILLevel = level;}
	u64  getBitmapId() const {return mBitmapId;}
	int  getNodeLevel() const {return mNodeLevel;}
	int  getIILLevel() const {return mIILLevel;}

};

#endif
