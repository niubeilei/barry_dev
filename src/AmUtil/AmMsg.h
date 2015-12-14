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
#ifndef Aos_AmUtil_AmMsg_h
#define Aos_AmUtil_AmMsg_h

#include "aosUtil/Types.h"
#include "AmUtil/AmMsgId.h"
#include "AmUtil/AmTagId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DynArray.h"

#include "AmUtil/Ptrs.h"
#include "UtilComm/ConnBuff.h"

// 
// Byte 0:   message ID
// Byte 1-2: Length (not include these two bytes)
// Byte 3-4: Transaction ID 
// Byte 5-8: Session ID
//
//
class AosAmMsg : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eInitBufSize = 200, 
		eIncrementSize = 101,
		eMaxBufLen = 1010101
	};

	struct TagEntry
	{
		int		mOffset;	// the offset from buffer position to Tag data position
		u16		mLen;		// the length of the tag's data, except the TagId and length itself

		TagEntry()
		:
		mOffset(-1), 
		mLen(0)
		{
		}
	};

private:
	OmnConnBuffPtr 	mBuff;
//	u8 				mMsgId;
//	u16 			mLen;
	char *			mContents;
//	uint 			mTransId;
//	uint 			mSessionId;
	TagEntry		mEntries[AosAmTagId::eMaxTagId];

public:
	AosAmMsg(const u32 initBufLen);
	AosAmMsg(const OmnConnBuff &buff);
	AosAmMsg(const OmnConnBuffPtr &buff);
	~AosAmMsg();

	bool setTransId(const u16 transId);
	u16  getTransId() const;
	bool setMsgId(const u8 msgId);
	u8   getMsgId() const;
	bool setSessionId(const u16 sessionId);
	u16  getSessionId() const;
	u16  getRespCode() const;
	OmnString getErrmsg() const;

	bool addTag(const AosAmTagId::E tagId, const OmnString &value);
	bool addTag(const AosAmTagId::E tagId, const u32 value);
	bool addTag(const AosAmTagId::E tagId, const u16 value);

	bool isResponse() const;
	bool isRespPositive() const;
	OmnConnBuffPtr getBuffer() const {return mBuff;}
	char * 	getData() const {return mBuff->getData();}
	u32		getDataLength() const {return mBuff->getDataLength();}

//	OmnString	getTagStr(const u8 tagId) const;
//	u32			getTagInt(const u8 tagId) const;
//	u16			getTagU16(const u8 tagId) const;
//	bool 		getTagU8(u8 &retVal, const u8 tagId) const;

	bool 		getTag(OmnString &retVal, const u8 tagId) const;
	bool 		getTag(u32 &retVal, const u8 tagId) const;
	bool 		getTag(u16 &retVal, const u8 tagId) const;
	bool 		getTag(u8 &retVal, const u8 tagId) const;
	void 		resetTags();

	bool 		parse();

private:
	//	static AosAmMsgPtr		createMsg(const OmnConnBuffPtr &conn);
};
#endif


