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
#ifndef AOS_DfmUtil_DfmDoc_h
#define AOS_DfmUtil_DfmDoc_h

#include "DfmUtil/Ptrs.h"
#include "UtilData/DfmDocType.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

#include <set>
#include <vector>
using namespace std;

class AosDfmDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Opr
	{
		eInvalidOpr = 0,

		eModify,
		eDelete,
		eNew,
		eSave,

		eMaxOpr
	};


private:
	OmnMutexPtr		mLock;
	u64				mDocid;
	AosDfmDocType::E mType;
	AosDfmDocHeaderPtr mHeader;

	AosBuffPtr		mBodyBuff;
	bool            mNeedCompress;
	u32				mOrigLen;
	u32 			mCompressLen;
	Opr				mOpr;

public:
	AosDfmDoc(
		const AosDfmDocType::E type,
		const bool regflag);
	AosDfmDoc(
		const u64 docid, 
		const AosDfmDocType::E type);
	~AosDfmDoc();

	static AosDfmDocPtr cloneDoc(const AosDfmDocType::E type, const u64 docid = 0);
	static AosDfmDocPtr cloneDoc(const AosDfmDocType::E type, const AosDfmDocHeaderPtr &header);

	AosDfmDocPtr 	copyDoc(bool copy_body = false);
	
	AosDfmDocHeaderPtr cloneDfmHeader(const u64 docid);
	AosDfmDocHeaderPtr cloneDfmHeader(
				const u64 docid,
				const AosBuffPtr &header_buff);

	// AosDfmDoc Interface.
	virtual AosDfmDocPtr clone(const u64 docid) = 0;
	virtual u32 getHeaderSize() = 0;
	virtual bool serializeFrom(
				const AosBuffPtr &header_buff,
				u32 &body_seq,
				u64 &body_off) = 0;
	virtual bool serializeTo(
				const AosBuffPtr &header_buff,
				const u32 body_seq,
				const u64 body_off) = 0;
	virtual bool getBodySeqOffFromBuff(
				const AosBuffPtr &header_buff,
				u32 &body_seq,
				u64 &body_off) = 0;
	virtual bool setBodySeqOffToBuff(
				const AosBuffPtr &header_buff,
				const u32 body_seq,
				const u64 body_off) = 0;
	// AosDfmDoc Interface End.
	
	AosDfmDocType::E getType(){ return mType; };
	static int getHeaderSize(const AosDfmDocType::E type);

	bool 	setBodySeqOff(const u32 seq, const u64 off);
	AosDfmDocHeaderPtr getHeader();
	bool	flushToHeaderBuff();

	bool	setBodyBuff(const AosBuffPtr &body_buff);
	AosBuffPtr getBodyBuff();
	void	cleanBodyBuff();

	void    setNeedCompress(const bool b) {mNeedCompress = b;}
	bool    needCompress() const {return mNeedCompress;}

	void 	setOrigLen(const u32 len){mOrigLen = len;}
	u32		getOrigLen(){return mOrigLen;}

	void 	setCompressLen(const u32 len) {mCompressLen = len;}
	u32		getCompressLen(){return mCompressLen;}
	
	void	setOpr(Opr b) { mOpr = b;}
	Opr		getOpr() {return mOpr;}
	
	u64		getDocid(){ return mDocid; };

	bool	setHeaderBuff(const AosBuffPtr &buff);
	AosBuffPtr getHeaderBuff();
	
	u32		getBodySeqno();
	u64		getBodyOffset();
	bool	isHeaderEmpty();	

private:
	bool 	registerDfmDoc(const AosDfmDocPtr &doc);
	bool 	initHeader();
	bool 	initByHeader(const AosDfmDocHeaderPtr &header);
	AosBuffPtr serializeToBuff(const u32 body_seq, const u64 body_off);

};

#endif

