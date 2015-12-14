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
// 2013/03/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_IILCache_DfmDocIILCache_h
#define AOS_IILCache_DfmDocIILCache_h

#include "DocFileMgr/Ptrs.h"
#include "DocFileMgr/DfmDoc.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/DocTypes.h"

class AosDfmDocIILCache : public AosDfmDoc 
{
public:
	enum
	{
		eHeaderSize = 32 
	};

	enum Type
	{
		eInvalidType,

		eIILDocids,
		eQueriedData,

		eMaxType
	};

private:
	u64			mIILID;
	u64			mTimestamp;
	Type		mType;

public:
	AosDfmDocIILCache(const u64 &docid, const Type type);
	~AosDfmDocIILCache();
	
	virtual AosDfmDocPtr clone(const u64 docid);
	virtual bool getBodySeqOff(                        
				const AosBuffPtr &header_buff,
				u32 &b_seqno,
				u64 &b_offset);
	virtual bool setBodySeqOff( 
			    const u32 b_seqno,
				const u64 b_offset);
	virtual void createHeaderBuffLocked();
	virtual u32 getHeaderSize(){ return eHeaderSize; };
	
	void setIILID(const u64 &iilid) {mIILID = iilid;}
	void setTimestamp(const u64 &t) {mTimestamp = t;}
	u64 getIILID() const {return mIILID;}
	Type getType() const {return mType;}
	u64 getTimestamp() const {return mTimestamp;}

};

#endif
#endif
