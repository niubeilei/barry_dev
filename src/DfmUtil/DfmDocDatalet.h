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
#ifndef AOS_DfmUtil_DfmDocDatalet_h
#define AOS_DfmUtil_DfmDocDatalet_h

#include "DfmUtil/DfmDoc.h"

#include <set>
using namespace std;

class AosDfmDocDatalet: public AosDfmDoc
{

public:	
	enum
	{
		// Header Format.
		eVersionOff = 0,
		eSiteidOff = eVersionOff + 4,
		eDocidOff = eSiteidOff + 4,

		eHeaderCommonInfoSize = eDocidOff + 8,		// 16 bytes.
		eHeaderNormalInfoSize = 16,
		
		eHeaderSize = 296,		// eHeaderSize == AosXmlDoc::eDocHeaderSize.
		
		eCurrentVersion = 11,	// moved from AosXmlDoc
	};

private:
	u32		mSiteid;
	u64		mDocDid;

public:
	AosDfmDocDatalet(const bool regflag);
	AosDfmDocDatalet(const u64 docid);
	AosDfmDocDatalet(
		const u64 loc_did,
		const u64 docid,
		const u32 siteid);
	~AosDfmDocDatalet();
	
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

	//void 	setCrtVersion();
	void 	setSiteid(const u32 siteid){ mSiteid = siteid; };
	void 	setDocid(const u64 &docid){ mDocDid = docid; };
	
private:	

};

#endif
