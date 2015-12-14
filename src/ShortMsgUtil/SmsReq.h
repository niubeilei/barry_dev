////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ShortMsgUtil_SmsReq_h
#define AOS_ShortMsgUtil_SmsReq_h

#include "Rundata/Rundata.h"
#include "ShortMsgUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"


struct AosSmsReq : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	int					fd;
	AosSmsReqPtr		mtPart;
	OmnString			iNumStr;
	OmnString			sContent;
	AosSmsReqPtr		prev;
	AosSmsReqPtr		next;

public:
	AosSmsReq();

	~AosSmsReq();

};

#endif
