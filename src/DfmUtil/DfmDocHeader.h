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
// 10/30/2012 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmDocHeader_h
#define AOS_DfmUtil_DfmDocHeader_h

#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosDfmDocHeader : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	u64				mDocid;
	u32				mBodySeqno;
	u64				mBodyOffset;
	AosBuffPtr  	mHeaderBuff;

public:
	AosDfmDocHeader(
			const u64 docid,
			const u32 body_seq,
			const u64 body_off,
			const AosBuffPtr &header_buff);
	~AosDfmDocHeader();

	u64		getDocid(){ return mDocid; };
	void 	setDocid(const u64 docid){mDocid = docid;};		//by White 2015-1-23
	void 	setBodySeqOff(const u32 seq, const u64 off);

	bool 	isEmpty();
	bool 	isSameBodySeqOff(const AosDfmDocHeaderPtr &header);

	AosBuffPtr getHeaderBuff();

	u32		getBodySeqno(){ return mBodySeqno; };
	u64		getBodyOffset(){ return mBodyOffset; };

	AosDfmDocHeaderPtr copy();

	bool	reset(const u32 body_seq,
				const u64 body_off,
				const AosBuffPtr &header_buff); 

	bool 	resetBuff(const AosBuffPtr &header_buff);

};

#endif

