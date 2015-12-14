////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerialTo.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SerialTo_h
#define Omn_Util_SerialTo_h

#include "aosUtil/Types.h"
#include "Message/MsgId.h"
#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/DateTime.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"

#include <string>
#include <iostream>

class OmnSerialTo : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnConnBuffPtr		mBuff;
	unsigned char *		mData;
	int					mDataLength;
	int					mCrtIndex;

public:
	OmnSerialTo();
	~OmnSerialTo();

	// 
	// Taken out by Chen Ding, 01/24/2008
	//
	// void			setBuff(const OmnConnBuffPtr &buff);
	
	const unsigned char *	getData() const {return mData;}
	int				getDataLength() const;
	void			setDataLength();
	OmnConnBuffPtr	getDataBuffer() const {return mBuff;}

	//
	// Serialize to functions
	//
	bool push(const OmnMsgId::E msgType);
	bool push(const char memberTag, const char *value);
	bool push(const char memberTag, const char value);
	bool push(const char memberTag, const int  value);
	bool push(const char memberTag, const bool value);
	bool push(const char memberTag, const uint  value);
	bool push(const char memberTag, const OmnString &value);
	bool push(const char memberTag, const OmnIpAddr &value);
	bool push(const char memberTag, const float &value);
	bool push(const char memberTag, const int64_t &value);
	bool push(const char memberTag, const u64 &value);
	bool push(const char memberTag, const OmnDateTime &value);
	bool push(const char memberTag, const OmnConnBuffPtr &buff);

	OmnSerialTo & operator << (const OmnMsgId::E msgType);
	OmnSerialTo & operator << (const char *value);
	OmnSerialTo & operator << (const char value);
	OmnSerialTo & operator << (const int  value);
	OmnSerialTo & operator << (const bool value);
	OmnSerialTo & operator << (const uint  value);
	OmnSerialTo & operator << (const OmnString &value);
	OmnSerialTo & operator << (const std::string &value);
	OmnSerialTo & operator << (const OmnIpAddr &value);
	OmnSerialTo & operator << (const float &value);
	OmnSerialTo & operator << (const int64_t &value);
	OmnSerialTo & operator << (const u64 &value);
	OmnSerialTo & operator << (const OmnDateTime &value);
	OmnSerialTo & operator << (const OmnConnBuffPtr &buff);
	OmnSerialTo & operator << (ostream & (*f)(ostream &outs));
};
#endif

