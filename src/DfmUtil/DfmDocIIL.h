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
#ifndef AOS_DfmUtil_DfmDocIIL_h
#define AOS_DfmUtil_DfmDocIIL_h

#include "DfmUtil/DfmDoc.h"
#include "SEUtil/SeTypes.h"


class AosDfmDocIIL : public AosDfmDoc 
{
public:
	enum
	{
		eIILTypeOff = 0,
		eIILIDOff = eIILTypeOff + 1,
		eWordIdOff = eIILIDOff + 8,
		eNumDocsOff = eWordIdOff + 8,
		eFlagOff = eNumDocsOff + 8,
		eHitCountOff = eFlagOff + 1,
		eVersionOff = eHitCountOff + 4,
		
		eHeaderCommonInfoSize = eVersionOff + 4,	// 34 byte.
		eHeaderNormalInfoSize = 16,

		eHeaderSize = 200,		// this eHeaderSize == AosIIL::eIILHeaderSize.
	
		ePersisBitOnVersionFlag = 0x80000000,	// moved from AosIIL.
	};

private:
	AosIILType	mIILType;
	u64			mIILID;	
	u64			mWordId;
	u32			mVersion;
	bool		mIsPersis;
	u32			mHitCount;
	int64_t		mNumDocs;
	char		mFlag;

public:
	AosDfmDocIIL(const bool regflag);
	AosDfmDocIIL(const u64 loc_did);
	AosDfmDocIIL(const u64 loc_did,
		const AosIILType iil_type,
		const u64 iilid,
		const u64 wordid,
		const u32 version,
		const bool is_persis,
		const u32 hit_count,
		const u32 num_docs,
		const char flag);
	~AosDfmDocIIL();
	
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


	void 	setIILType(const AosIILType type){ mIILType = type; };
	void 	setIILID(const u64 &iilid){ mIILID = iilid; };
	void 	setWordId(const u64 &wordid){ mWordId = wordid; };
	void 	setVersion(const u32 version){ mVersion = version; };
	void 	setIsPersis(const bool is_persis){ mIsPersis = is_persis; };
	void 	setHitCount(const u32 hit_count){ mHitCount = hit_count; };
	void 	setNumDocs(const int64_t &num_docs){ mNumDocs = num_docs; };
	void 	setFlag(const char flag){ mFlag = flag; };
	
	AosIILType	getIILType() {return mIILType;}
	u64		getIILID() {return mIILID;}
	u64		getWordId() {return mWordId;}
	u32		getVersion() {return mVersion;}
	bool	getPersis() {return mIsPersis;}
	u32		getHitCount() {return mHitCount;}
	int64_t	getNumDocs() {return mNumDocs;}
	char	getFlag() {return mFlag;}
	
};

#endif
