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
#ifndef Aos_SysIDO_SeqnoIDOSvr_h
#define Aos_SysIDO_SeqnoIDOSvr_h

#include "SEInterfaces/JimoCallPackage.h"
#include "SysIDOs/Ptrs.h"


class AosRundata;
class AosJimoCall;
class OmnMutex;

class AosSeqnoIDOSvr : public AosJimoCallPackage
{
	OmnDefineRCObject;

private:
	typedef std::hash_map<const OmnString, AosSeqnoIDOObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef std::hash_map<const OmnString, AosSeqnoIDOObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t 		mIDOMap;
	OmnMutex *	mLock;

public:
	AosSeqnoIDOSvr(const int version);
	~AosSeqnoIDOSvr();

	virtual AosJimoPtr cloneJimo() const;
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const;

private:
	u64 createByName(AosRundata *rdata, AosJimoCall &jimo_call);
	u64 nextSeqno(AosRundata *rdata, AosJimoCall &jimo_call);

};
#endif
