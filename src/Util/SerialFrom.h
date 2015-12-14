////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SerialFrom.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SerialFrom_h
#define Omn_Util_SerialFrom_h

#include "aosUtil/Types.h"
#include "Message/MsgId.h"
#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"

#include <string>

class OmnDateTime;


class OmnSerialFrom : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnConnBuffPtr		mBuff;
	char *				mData;
	int					mDataLength;
	int					mCrtIndex;
	int					mExpectedLength;
	bool				mComplete;

public:
	OmnSerialFrom();
	OmnSerialFrom(const OmnConnBuffPtr &buff);
	~OmnSerialFrom();


	const char *	getData() const {return mData;}
	int				getDataLength() const {return mDataLength;}
	void			reset() {mCrtIndex = 0;}
	OmnConnBuffPtr	getDataBuff() const {return mBuff;}
	// void			setBuffer(const OmnConnBuffPtr &buff);
	bool			appendBuffer(const OmnConnBuffPtr &buff);
	bool			isComplete() const {return mComplete;}
	void			resetBuffer();

	//
	// Serialize From Functions
	//
	OmnMsgId::E	popMsgId();
	bool		pop(const char memberTag, char *value);
	bool		pop(const char memberTag, char &value);
	bool		pop(const char memberTag, int &value);
	bool		pop(const char memberTag, bool &value);
	bool		pop(const char memberTag, uint &value);
	bool		pop(const char memberTag, OmnString &value);
	bool		pop(const char memberTag, std::string &value);
	bool		pop(const char memberTag, OmnIpAddr &value);
	bool		pop(const char memberTag, float &value);
	bool		pop(const char memberTag, int64_t &value);
	bool		pop(const char memberTag, u64 &value);
	bool		pop(const char memberTag, OmnDateTime &value);
	bool		pop(const char memberTag, OmnConnBuffPtr &buff);

	OmnSerialFrom & operator >> (char *value);
	OmnSerialFrom & operator >> (char &value);
	OmnSerialFrom & operator >> (int &value);
	OmnSerialFrom & operator >> (bool &value);
	OmnSerialFrom & operator >> (uint &value);
	OmnSerialFrom & operator >> (OmnString &value);
	OmnSerialFrom & operator >> (std::string &value);
	OmnSerialFrom & operator >> (OmnIpAddr &value);
	OmnSerialFrom & operator >> (float &value);
	OmnSerialFrom & operator >> (int64_t &value);
	OmnSerialFrom & operator >> (u64 &value);
	OmnSerialFrom & operator >> (OmnDateTime &value);
	OmnSerialFrom & operator >> (OmnConnBuffPtr &buff);

private:
	char * retrieveStr(int &len);
};
#endif

