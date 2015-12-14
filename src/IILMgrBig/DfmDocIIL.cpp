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
#include "IILMgrBig/DfmDocIIL.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


bool AosDfmDocIIL::smShowLog = true;

AosDfmDocIIL::AosDfmDocIIL(const u64 docid)
:
AosDfmDoc(docid, eIIL, eHeaderSize)
{
}

/*
AosDfmDocIIL::AosDfmDocIIL(const u64 docid, const AosBuffPtr &header_buff)
:
AosDfmDoc(docid, eIIL, eHeaderSize, header_buff)
{
	initBodySeqOff(mHeaderBuff);
}
*/

AosDfmDocIIL::~AosDfmDocIIL()
{
}

	
AosDfmDocPtr
AosDfmDocIIL::clone(const u64 docid)
{
	return OmnNew AosDfmDocIIL(docid);	
}


bool
AosDfmDocIIL::initBodySeqOff(const AosBuffPtr &header_buff)
{
	// This func maybe called by, initHeaderBuff.
	// saveCache. and the constructor.
	//
	// when called this func. must locked.
	aos_assert_r(header_buff, false);
	if(header_buff->dataLen() <=eHeaderCommonInfoSize)	return true;
	
	header_buff->setCrtIdx(eHeaderCommonInfoSize);
	header_buff->getU32(0);
	mBodySeqno = header_buff->getU32(0);
	mBodyOffset = header_buff->getU64(0);

	return true;
}


bool
AosDfmDocIIL::resetBodySeqOff(const u32 seq, const u64 offset)
{
	aos_assert_r(mHeaderBuff, false);
	
	mLock->lock();
	
	mBodySeqno = seq;
	mBodyOffset = offset;

	mHeaderBuff->setCrtIdx(eHeaderCommonInfoSize);
	mHeaderBuff->setU32(0);
	mHeaderBuff->setU32(seq);
	mHeaderBuff->setU64(offset);

	mLock->unlock();
	return true;
}


void
AosDfmDocIIL::setIILID(const u64 iilid)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eIILIDOff);
	mHeaderBuff->setU64(iilid);
	mLock->unlock();
}


void
AosDfmDocIIL::setWordId(const u64 wordid)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eWordIdOff);
	mHeaderBuff->setU64(wordid);
	mLock->unlock();
}


void
AosDfmDocIIL::setIILType(const AosIILType type)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eIILTypeOff);
	mHeaderBuff->setU8((u8)type);
	//OmnScreen << "ktttttttt get iiltype:"
	//		<< "; type:" << type
	//		<< "; docid:" << mDocid 
	//		<< endl;
	mLock->unlock();
}


void
AosDfmDocIIL::setVersion(const u32 version, const bool is_persis)
{
	u32 ver = version;
	if(is_persis)
	{
		ver = ver | ePersisBitOnVersionFlag;
	}
	
	mLock->lock();
	mHeaderBuff->setCrtIdx(eVersionOff);
	mHeaderBuff->setU32(ver);
	mLock->unlock();
}


void
AosDfmDocIIL::setHitCount(const u32 hit_count)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eHitCountOff);
	mHeaderBuff->setU32(hit_count);
	mLock->unlock();
}


void
AosDfmDocIIL::setNumDocs(const u32 num_docs)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eNumDocsOff);
	mHeaderBuff->setU32(num_docs);
	mLock->unlock();
}

void
AosDfmDocIIL::setFlag(const char flag)
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eFlagOff);
	mHeaderBuff->setChar(flag);
	mLock->unlock();
}
	

AosIILType	
AosDfmDocIIL::getIILType()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eIILTypeOff);
	AosIILType type = (AosIILType)mHeaderBuff->getU8(0);
	//OmnScreen << "ktttttttt get iiltype:"
	//		<< "; type:" << type
	//		<< "; docid:" << mDocid 
	//		<< endl;
	mLock->unlock();
	
	return type;
}


u64
AosDfmDocIIL::getIILID()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eIILIDOff);
	u64 iilid = mHeaderBuff->getU64(0);
	mLock->unlock();
	
	return iilid;
}


u64
AosDfmDocIIL::getWordId()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eWordIdOff);
	u64 wordid = mHeaderBuff->getU64(0);
	mLock->unlock();
	
	return wordid;
}


u32
AosDfmDocIIL::getNumDocs()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eNumDocsOff);
	u32 num_doc = mHeaderBuff->getU32(0);
	mLock->unlock();
	
	return num_doc;
}


char
AosDfmDocIIL::getFlag()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eFlagOff);
	char flag = mHeaderBuff->getChar(0);
	mLock->unlock();
	
	return flag;
}


u32
AosDfmDocIIL::getHitCount()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eHitCountOff);
	u32 hit_count = mHeaderBuff->getU32(0);
	mLock->unlock();
	
	return hit_count;
}


u32
AosDfmDocIIL::getVersion()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eVersionOff);
	u32 ver = mHeaderBuff->getU32(0);
	mLock->unlock();
	
	return ver & ~ePersisBitOnVersionFlag;
}


bool
AosDfmDocIIL::getPersis()
{
	mLock->lock();
	mHeaderBuff->setCrtIdx(eVersionOff);
	u32 ver = mHeaderBuff->getU32(0);
	mLock->unlock();
	
	return ver & ePersisBitOnVersionFlag;
}

