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
#ifndef AOS_DfmUtil_DfmDocNorm_h
#define AOS_DfmUtil_DfmDocNorm_h

#include "DfmUtil/DfmDoc.h"


class AosDfmDocNorm : public AosDfmDoc 
{
	enum
	{
		eHeaderSize = 12,
	};

public:
	AosDfmDocNorm(const u64 docid);
	AosDfmDocNorm(const bool regflag);
	virtual ~AosDfmDocNorm();

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

};

#endif
