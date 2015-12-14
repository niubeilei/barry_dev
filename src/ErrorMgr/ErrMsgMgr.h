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
//
// Modification History:
// 08/22/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ErrorMgr_ErrorMsg_h
#define Aos_ErrorMgr_ErrorMsg_h

#include "ErrorMgr/ErrmsgId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Locale.h"


class AosErrMsgMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosLocale::E		mLocale;
	OmnString			mMsgs[AosErrmsgId::eMaxErrorCode];

public:
	AosErrMsgMgr(const AosLocale::E locale);
	~AosErrMsgMgr();

	OmnString getErrmsg(const AosErrmsgId::E id);

private:
	bool init();
};
#endif
