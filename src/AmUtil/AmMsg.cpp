////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 3/31/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "AmUtil/AmMsg.h"

#include "Alarm/Alarm.h"
#include "AmUtil/AmUtil.h"
#include "alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

#define AM_MIN_MSG_SZ 9
#define AM_MSG_HEADER_SZ 9


AosAmMsg::AosAmMsg(const u32 initBufSize)
{
	u32 buffSize = (initBufSize > AM_MIN_MSG_SZ ? initBufSize : AM_MIN_MSG_SZ);
	mBuff = OmnNew OmnConnBuff(buffSize);
	aos_assert(mBuff);
	mBuff->setDataLength(AM_MSG_HEADER_SZ);
	mContents = mBuff->getBuffer();

	// set data length to buffer
	AosAmUtil::setU16(&(mBuff->getData()[1]), mBuff->getDataLength() - 3);
}


AosAmMsg::AosAmMsg(const OmnConnBuff &buff)
//:
//mBuff(buff.getBuffer(), buff.getBufferLength()),
//mContents(mBuff.getData())
{
	mBuff = OmnNew OmnConnBuff(buff);
	mContents = mBuff->getData();
	parse();
}


AosAmMsg::AosAmMsg(const OmnConnBuffPtr &buff)
:
mBuff(buff)
{
	aos_assert(mBuff);
	mContents = mBuff->getData();
	parse();
}


AosAmMsg::~AosAmMsg()
{
//	mLen = 0;
}


bool
AosAmMsg::setTransId(const u16 transId)
{
    aos_assert_r((AosAmUtil::setU16(&mContents[3], transId)) > 0, false);
//	mTransId = transId;
	return true;
}


u16
AosAmMsg::getTransId() const
{
	return AosAmUtil::getU16(&mContents[3]);
}


bool
AosAmMsg::setSessionId(const u16 sessionId)
{
    aos_assert_r((AosAmUtil::setU16(&mContents[5], sessionId)) > 0, false);
//	mSessionId = sessionId;
	return true;
}


u16
AosAmMsg::getSessionId() const
{
	return AosAmUtil::getU16(&mContents[5]);
}


bool
AosAmMsg::setMsgId(const u8 msgId)
{
	mBuff->getData()[0] = msgId;
//	mMsgId = msgId;
	return true;
}


u8
AosAmMsg::getMsgId() const
{
	return ((u8)mContents[0]);
}


bool
AosAmMsg::addTag(const AosAmTagId::E tagId, const OmnString &value)
{
	// 
	// Byte i:  		tagId
	// Byte i+1, 2: 	Tag Length
	// Byte i+3, ..:	Tag value
	//
	OmnConnBuff buff(value.length() + 3);
	char *bb = buff.getBuffer();
	bb[0] = tagId;
	AosAmUtil::setU16(&bb[1], value.length());
	memcpy(&bb[3], value.data(), value.length());
	buff.setDataLength(value.length() + 3);

	mEntries[tagId].mLen 	= value.length();
	mEntries[tagId].mOffset = mBuff->getDataLength() + 3;

	// set data length to buffer
	AosAmUtil::setU16(&(mBuff->getData()[1]), mBuff->getDataLength() - 3 + value.length() + 3);

	return mBuff->append(buff);
}


bool
AosAmMsg::addTag(const AosAmTagId::E tagId, const u32 value)
{
	// 
	// Byte i:  		tagId
	// Byte i+1, 2: 	Tag Length
	// Byte i+3, ..:	Tag value
	//
	OmnConnBuff buff(4 + 3);
	char *bb = buff.getBuffer();
	bb[0] = tagId;
	AosAmUtil::setU16(&bb[1], 4);
	AosAmUtil::setU32(&bb[3], value);
	buff.setDataLength(4 + 3);

	mEntries[tagId].mLen 	= sizeof(u32);
	mEntries[tagId].mOffset = mBuff->getDataLength() + 3;

	// set data length to buffer
	AosAmUtil::setU16(&(mBuff->getData()[1]), mBuff->getDataLength() - 3 + 4 + 3);

	return mBuff->append(buff);
}


bool
AosAmMsg::addTag(const AosAmTagId::E tagId, const u16 value)
{
	// 
	// Byte i:  		tagId
	// Byte i+1, 2: 	Tag Length
	// Byte i+3, ..:	Tag value
	//
	OmnConnBuff buff(2 + 3);
	char *bb = buff.getBuffer();
	bb[0] = tagId;
	AosAmUtil::setU16(&bb[1], 2);
	AosAmUtil::setU16(&bb[3], value);
	buff.setDataLength(2 + 3);

	mEntries[tagId].mLen 	= sizeof(u16);
	mEntries[tagId].mOffset = mBuff->getDataLength() + 3;

	// set data length to buffer
	AosAmUtil::setU16(&(mBuff->getData()[1]), mBuff->getDataLength() - 3 + 2 + 3);

	return mBuff->append(buff);
}


bool 
AosAmMsg::parse()
{
	// 
	// Assume the buff length is at least 1 bytes long
	// 
	// 
	// Byte 0:   message ID
	// Byte 1-2: Length (not include these two bytes)
	// Byte 3-4: Transaction ID 
	// Byte 5-8: Session ID
	//
	u16 len = AosAmUtil::getU16(&mContents[1]);
	aos_assert_r(len == (u16)(mBuff->getDataLength() - 3), false);

//	mMsgId = (AosAmMsgId::E) mContents[0];
//	mTransId = AosAmUtil::getU16(&mContents[3]);
//	mSessionId = AosAmUtil::getU32(&mContents[5]);

	// 
	// Load data to Tag Array
	//
	u16 offset = 9;
	while(offset < len)
	{
		u8 tagId = mContents[offset];
		mEntries[tagId].mLen 	= AosAmUtil::getU16(&mContents[offset+1]);
		mEntries[tagId].mOffset = offset + 3;
		offset 				   += mEntries[tagId].mLen + 3;
	}

	return true;
}


bool	
AosAmMsg::getTag(OmnString &retVal, const u8 tagId) const
{
	if (mEntries[tagId].mOffset < 0) return false;

	retVal.assign(&mContents[mEntries[tagId].mOffset], 
				   mEntries[tagId].mLen);
	return true;
}


bool			
AosAmMsg::getTag(u32 &retVal, const u8 tagId) const
{
	if (mEntries[tagId].mOffset < 0)  return false;
	if (mEntries[tagId].mLen 	!= 4) return false;

//	OmnString str(&mContents[mEntries[tagId].mOffset], 
//				   mEntries[tagId].mLen);
	retVal = AosAmUtil::getU32(&mContents[mEntries[tagId].mOffset]);
	return true;
}


bool			
AosAmMsg::getTag(u16 &retVal, const u8 tagId) const
{
	if (mEntries[tagId].mOffset < 0)  return false;
	if (mEntries[tagId].mLen 	!= 2) return false;

//	OmnString str(&mContents[mEntries[tagId].mOffset], 
//				   mEntries[tagId].mLen);
	retVal = AosAmUtil::getU16(&mContents[mEntries[tagId].mOffset]);
	return true;
}


bool 
AosAmMsg::getTag(u8 &retVal, const u8 tagId) const
{
	if (mEntries[tagId].mOffset < 0)  return false;
	if (mEntries[tagId].mLen 	!= 1) return false;

	retVal = mContents[mEntries[tagId].mOffset];
	return true;
}



void 
AosAmMsg::resetTags()
{
	if(!mBuff) return ;

	char oldHeadBuff[9];
	memcpy(oldHeadBuff, mBuff->getData(), 9);
	mBuff->setDataLength(9);
	mContents = mBuff->getData();
	AosAmUtil::setU16(&mContents[1], 6);

	// Byte 0:   message ID
	// Byte 1-2: Length (not include these two bytes)
	// Byte 3-4: Transaction ID 
	// Byte 5-8: Session ID
	//
//	mBuff->setDataLength(9);
//	mContents = mBuff->getData();
//	mContents[0] = mMsgId;
//	AosAmUtil::setU16(&mContents[1], 6);
//	AosAmUtil::setU16(&mContents[3], mTransId);
//	AosAmUtil::setU16(&mContents[5], mSessionId);

	for(int i = 0; i < AosAmTagId::eMaxTagId; i++)
	{
		mEntries[i].mOffset = -1; 
		mEntries[i].mLen 	= 0;
	}
}


bool
AosAmMsg::isRespPositive() const
{
	u16 respCode;
	if(getTag(respCode, AosAmTagId::eRsltCode))
	{
		return (AosAmRespCode::eAllowed == respCode);
	}
	else
	{
		return false;
	}
}


bool 
AosAmMsg::isResponse() const
{
	return ((getMsgId() == AosAmMsgId::eResponse) || (getMsgId() == AosAmMsgId::eCacheResponse));
}


u16 
AosAmMsg::getRespCode() const
{
	u16 respCode;
	if(getTag(respCode, AosAmTagId::eRsltCode))
	{
		return respCode;
	}
	else
	{
		return AosAmRespCode::eInternalError;
	}
}


OmnString 
AosAmMsg::getErrmsg() const
{
	OmnString errmsg;
	if(getTag(errmsg, AosAmTagId::eErrmsg))
	{
		return errmsg;
	}
	else
	{
		return OmnString("Fail to get error message. ");
	}
}
