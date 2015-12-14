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
// handle the SEServer send request to MsgServer 
//
//
//		SYBCHAR = 47,           /* 0x2F */
//		SYBVARCHAR = 39,        /* 0x27 */
//		SYBINTN = 38,           /* 0x26 */
//		SYBINT1 = 48,           /* 0x30 */
//		SYBINT2 = 52,           /* 0x34 */
//		SYBINT4 = 56,           /* 0x38 */
//		SYBINT8 = 127,          /* 0x7F */
//		SYBFLT8 = 62,           /* 0x3E */
//		SYBDATETIME = 61,       /* 0x3D */
//		SYBBIT = 50,            /* 0x32 */
//		SYBBITN = 104,          /* 0x68 */
//		SYBTEXT = 35,           /* 0x23 */
//		SYBNTEXT = 99,          /* 0x63 */
//		SYBIMAGE = 34,          /* 0x22 */
//		SYBMONEY4 = 122,        /* 0x7A */
//		SYBMONEY = 60,          /* 0x3C */
//		SYBDATETIME4 = 58,      /* 0x3A */
//		SYBREAL = 59,           /* 0x3B */
//		SYBBINARY = 45,         /* 0x2D */
//		SYBVOID = 31,           /* 0x1F */
//		SYBVARBINARY = 37,      /* 0x25 */
//		SYBNUMERIC = 108,       /* 0x6C */
//		SYBDECIMAL = 106,       /* 0x6A */
//		SYBFLTN = 109,          /* 0x6D */
//		SYBMONEYN = 110,        /* 0x6E */
//		SYBDATETIMN = 111,      /* 0x6F */
//		SYBNVARCHAR = 103       /* 0x67 */
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SyBaseMgr_Column_h
#define AOS_SyBaseMgr_Column_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "SyBaseMgr/Ptrs.h"

#include <sybfront.h>
#include <sybdb.h>
#include <vector>
#include <map>
class AosColumn : public OmnRCObject
{
	OmnDefineRCObject;
	friend class AosSyBaseMgr;
private:
	OmnString		mColName;
	OmnString		mColBuffer;
	int 			mColType;
	int 			mColSize;
	int 			mColStatus;
public:
	AosColumn();
	~AosColumn();

	void setColName(const OmnString &colname);
	void setColBuffer(const OmnString &colbuffer);
	void setColType(const int coltype);
	void setColSize(const int colsize);
	void setColStatus(const int colstatus);

	OmnString getColName();
	OmnString getColValue();
	OmnString getColBuffer();
	OmnString& getColBufferRef();
	int getColType();
	int getColSize();
	int getColStatus();
	int& getColStatusRef();
};
#endif

