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
// version      u32
// siteid       u32
// groupid		u32
// seqno		u32
// offset		u64
// Modification History:
// 01/23/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmGroupedDoc_h
#define AOS_DfmUtil_DfmGroupedDoc_h

#include "DfmUtil/DfmDoc.h"


class AosDfmGroupedDoc: public AosDfmDoc
{

public:	
	enum
	{
		eVersionOffset = 0,
		eSiteidOffset = eVersionOffset + 4,
		eGroupidOffset = eSiteidOffset + 4,
		//eHeaderCommonInfoSize = eGroupidOffset + 4,	
		eHeaderCommonInfoSize = eGroupidOffset + 8,	
		
		//eGroupDocsHeaderSize = 24, 	
		eGroupDocsHeaderSize = 28, 	
		
		eCurrentVersion = 11	
	};

private:
	u32		mSiteid;
	u64		mGroupId;

public:
	AosDfmGroupedDoc(const bool regflag);
	AosDfmGroupedDoc(const u64 group_id);
	AosDfmGroupedDoc(
		const u32 siteid,
		const u64 group_id);
	~AosDfmGroupedDoc();
	
	// AosDfmDoc Interface.
	virtual AosDfmDocPtr clone(const u64 docid);
	virtual u32 getHeaderSize(){ return eGroupDocsHeaderSize; };

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

	void 	setCrtVersion();
	void 	setSiteid(const u32 siteid){ mSiteid = siteid; };
	void 	setGroupid(const u64 &groupid){ mGroupId = groupid; };
	
};

#endif
