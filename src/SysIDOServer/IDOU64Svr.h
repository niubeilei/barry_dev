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
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysIDO_U64IDOSvr_h
#define Aos_SysIDO_U64IDOSvr_h

#include "SEInterfaces/JimoCallPackage.h"
#include "SysIDOs/Ptrs.h"


class AosRundata;
class AosJimoCall;
class OmnMutex;

class AosU64IDOSvr : public AosJimoCallPackage
{
	OmnDefineRCObject;

private:
	typedef std::hash_map<const OmnString, AosU64IDOObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef std::hash_map<const OmnString, AosU64IDOObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t 		mIDOMap;
	OmnMutex *	mLock;

public:
	AosU64IDOSvr(const int version);
	~AosU64IDOSvr();

	u64 nextSeqno(AosRundata *rdata);

	virtual AosJimoPtr cloneJimo() const;
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;

private:
	bool createByName(AosRundata *rdata, AosJimoCall &jimo_call);
	bool get(AosRundata *rdata, AosJimoCall &jimo_call);
	bool set(AosRundata *rdata, AosJimoCall &jimo_call);
	u64 nextSeqno(AosRundata *rdata, AosJimoCall &jimo_call);

};
#endif
