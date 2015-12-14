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

#include "DfmUtil/DfmDoc.h"

#include "DfmUtil/DfmDocHeader.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"

extern AosDfmDocPtr sgDfmDocs[AosDfmDocType::eMax];

AosDfmDoc::AosDfmDoc(
		const AosDfmDocType::E type,
		const bool regflag)
:
mLock(OmnNew OmnMutex()),
mDocid(0),
mType(type),
mNeedCompress(false),
mOrigLen(0),
mCompressLen(0),
mOpr(eInvalidOpr)
{
	if(regflag)
	{
		AosDfmDocPtr thisptr(this, false);
		registerDfmDoc(thisptr);
	}
}

AosDfmDoc::AosDfmDoc(
		const u64 docid, 
		const AosDfmDocType::E type)
:
mLock(OmnNew OmnMutex()),
mDocid(docid),
mType(type),
mNeedCompress(false),
mOrigLen(0),
mCompressLen(0),
mOpr(eInvalidOpr)
{
}


AosDfmDoc::~AosDfmDoc()
{
}


bool
AosDfmDoc::registerDfmDoc(const AosDfmDocPtr &doc)
{
	AosDfmDocType::E type = doc->mType;

	aos_assert_r(AosDfmDocType::isValid(type), false);
	aos_assert_r(!sgDfmDocs[type], false);
	sgDfmDocs[type] = doc;
	return true;
}


AosDfmDocPtr
AosDfmDoc::cloneDoc(const AosDfmDocType::E type, const u64 docid)
{
	aos_assert_r(AosDfmDocType::isValid(type), 0);
	aos_assert_r(sgDfmDocs[type], 0);
	
	return sgDfmDocs[type]->clone(docid);
}


AosDfmDocPtr
AosDfmDoc::cloneDoc(const AosDfmDocType::E type, const AosDfmDocHeaderPtr &header)
{
	aos_assert_r(header, 0);
	AosDfmDocPtr dfm_doc = cloneDoc(type, header->getDocid());
	aos_assert_r(dfm_doc, 0);
	
	bool rslt = dfm_doc->initByHeader(header);
	aos_assert_r(rslt, 0);
	return dfm_doc;
}


AosDfmDocPtr
AosDfmDoc::copyDoc(bool copy_body)
{
	// this func will copy mHeader. 
	AosDfmDocPtr dfm_doc = clone(mDocid);

	AosDfmDocHeaderPtr new_header = mHeader->copy();
	bool rslt = dfm_doc->initByHeader(new_header);
	aos_assert_r(rslt, 0);

	mLock->lock();
	if(copy_body)
	{
		AosBuffPtr buff = OmnNew AosBuff(mBodyBuff->dataLen() AosMemoryCheckerArgs);	
		buff->setBuff(mBodyBuff);
		
		dfm_doc->mBodyBuff = buff;
	}
	else
	{
		dfm_doc->mBodyBuff = mBodyBuff;
	}

	dfm_doc->mNeedCompress = mNeedCompress;
	dfm_doc->mOrigLen = mOrigLen;
	dfm_doc->mCompressLen = mCompressLen;
	dfm_doc->mOpr = mOpr;
	
	mLock->unlock();

	return dfm_doc;
}

bool
AosDfmDoc::initByHeader(const AosDfmDocHeaderPtr &header)
{
	aos_assert_r(header, false);
	
	mHeader = header;
	AosBuffPtr header_buff = mHeader->getHeaderBuff();
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(0);
	u32 body_seq = 0;
	u64 body_off = 0;
	bool rslt = serializeFrom(header_buff, body_seq, body_off);
	aos_assert_r(rslt, false);
	
	aos_assert_r(header->getBodySeqno() == body_seq &&
			header->getBodyOffset() == body_off, false);
	return true;
}


AosDfmDocHeaderPtr
AosDfmDoc::cloneDfmHeader(const u64 docid)
{
	// this new header_buff is empty.
	u32 header_size = getHeaderSize();
	AosBuffPtr header_buff = OmnNew AosBuff(header_size AosMemoryCheckerArgs);
	memset(header_buff->data(), 0, header_size);

	AosDfmDocHeaderPtr header = OmnNew AosDfmDocHeader(docid, 0, 0, header_buff);	
	return header;
}


AosDfmDocHeaderPtr
AosDfmDoc::cloneDfmHeader(
		const u64 docid,
		const AosBuffPtr &header_buff)
{
	aos_assert_r(header_buff->dataLen() <= getHeaderSize(), 0);

	u32 body_seqno;
	u64 body_offset;
	bool rslt = getBodySeqOffFromBuff(header_buff, body_seqno, body_offset);
	aos_assert_r(rslt, 0);
	
	AosDfmDocHeaderPtr header = OmnNew AosDfmDocHeader(docid,
			body_seqno, body_offset, header_buff);	
	return header;
}


bool
AosDfmDoc::setBodySeqOff(const u32 seq, const u64 off)
{
	// just change this header's body_seq and body_off.
	mLock->lock();
	if(!mHeader)	initHeader();
	aos_assert_rl(mHeader, mLock, 0);

	mHeader->setBodySeqOff(seq, off);

	AosBuffPtr header_buff = mHeader->getHeaderBuff();
	bool rslt = setBodySeqOffToBuff(header_buff, seq, off);
	aos_assert_rl(rslt, mLock, false);
	
	mLock->unlock();
	return true;
}


AosDfmDocHeaderPtr
AosDfmDoc::getHeader()
{
	mLock->lock();
	if(!mHeader)	initHeader();
	mLock->unlock();
	
	aos_assert_r(mHeader, 0);
	return mHeader;
}


bool
AosDfmDoc::initHeader()
{
	aos_assert_r(!mHeader, false);
	
	AosBuffPtr header_buff = serializeToBuff(0, 0);
	aos_assert_r(header_buff, false);
	
	mHeader = OmnNew AosDfmDocHeader(mDocid, 0, 0, header_buff);
	return true;
}


AosBuffPtr
AosDfmDoc::serializeToBuff(const u32 body_seq, const u64 body_off)
{
	AosBuffPtr header_buff = OmnNew AosBuff(getHeaderSize() AosMemoryCheckerArgs);
	memset(header_buff->data(), 0, header_buff->buffLen());

	bool rslt = serializeTo(header_buff, body_seq, body_off);
	aos_assert_r(rslt, 0);

	return header_buff;	
}


bool
AosDfmDoc::setBodyBuff(const AosBuffPtr &body_buff)
{
	mLock->lock();	
	mBodyBuff = body_buff;
	mLock->unlock();
	
	return true;
}


AosBuffPtr
AosDfmDoc::getBodyBuff()
{
	AosBuffPtr body_buff;
	
	mLock->lock();
	if(!mBodyBuff && mHeader && !mHeader->isEmpty())
	{
		// means this body is in file. not readed yet.
		// need read from file.
		OmnAlarm << "OmnNotImplementedYet;" << enderr;
		mLock->unlock();
		return 0;
	}
	body_buff = mBodyBuff;
	mLock->unlock();
	
	return body_buff;
}

void
AosDfmDoc::cleanBodyBuff()
{
	// Ketty IIL will use this func.
	mLock->lock();	
	mBodyBuff = 0;
	mLock->unlock();
}


bool
AosDfmDoc::setHeaderBuff(const AosBuffPtr &buff)
{
	u32 body_seqno;
	u64 body_offset;
	bool rslt = getBodySeqOffFromBuff(buff, 
			body_seqno, body_offset);
	aos_assert_r(rslt, 0);
	
	mLock->lock();
	if(!mHeader)
	{
		mHeader = OmnNew AosDfmDocHeader(mDocid,
				body_seqno, body_offset, buff); 
	}
	else
	{
		mHeader->reset(body_seqno, body_offset, buff);
	}

	rslt = initByHeader(mHeader);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	return true;
}


bool
AosDfmDoc::flushToHeaderBuff()
{
	// iil maybe reset DfmDocIIL's mem data. such as mNumDocs.
	// so if mHeader is exist. need re serialize header_buff.
	mLock->lock();
	if(!mHeader) initHeader();
	aos_assert_rl(mHeader, mLock, false);
	
	AosBuffPtr header_buff = serializeToBuff(mHeader->getBodySeqno(), mHeader->getBodyOffset());
	mHeader->resetBuff(header_buff);
	mLock->unlock();
	
	return true;
}


AosBuffPtr
AosDfmDoc::getHeaderBuff()
{
	mLock->lock();
	if(!mHeader) initHeader();
	aos_assert_rl(mHeader, mLock, 0);
	
	AosBuffPtr header_buff = mHeader->getHeaderBuff();
	aos_assert_rl(header_buff, mLock, 0);
	mLock->unlock();

	return header_buff;	
}


u32
AosDfmDoc::getBodySeqno()
{
	mLock->lock();
	if(!mHeader)	initHeader();
	mLock->unlock();
	
	aos_assert_r(mHeader, 0);
	return mHeader->getBodySeqno();
}


u64	
AosDfmDoc::getBodyOffset()
{
	mLock->lock();
	if(!mHeader)	initHeader();
	mLock->unlock();
	
	aos_assert_r(mHeader, 0);
	return mHeader->getBodyOffset();
}


bool
AosDfmDoc::isHeaderEmpty()
{
	mLock->lock();
	if(!mHeader)	initHeader();
	mLock->unlock();
	
	aos_assert_r(mHeader, 0);
	return mHeader->isEmpty();
}


// Gavin, 2015/10/13
int
AosDfmDoc::getHeaderSize(const AosDfmDocType::E type)
{
	aos_assert_r(AosDfmDocType::isValid(type), -1);
	switch(type)		//FIXME:DO NOT hard code this!!!!!
	{
	case AosDfmDocType::eNormal:
		return 12;
	case AosDfmDocType::eIIL:
		return 200;
	case AosDfmDocType::eGroupedDoc:
		return 28;
	case AosDfmDocType::eBitmap:
		return 50;
	case AosDfmDocType::eIILCache:
		return 32;
	case AosDfmDocType::eDatalet:		// Chen Ding, 2013/06/12
		return 296;
	default:
		OmnAlarm << "invalid type:" << type << enderr;
		return -1;
	}

	//if (!sgDfmDocs[type]) return -1;
	//return sgDfmDocs[type]->getHeaderSize();
}


