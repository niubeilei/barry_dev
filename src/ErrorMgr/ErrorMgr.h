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
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ErrorMgr_ErrorMgr_h
#define Aos_ErrorMgr_ErrorMgr_h

#include "ErrorMgr/Ptrs.h"
#include "ErrorMgr/ErrmsgId.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/Locale.h"


OmnDefineSingletonClass(AosErrorMgrSingleton,
						AosErrorMgr,
						AosErrorMgrSelf,
						OmnSingletonObjId::eErrorMgr,
						"ErrorMgr");

class AosErrorMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mDirname;
	AosErrMsgMgrPtr	mErrMsgMgr[AosLocale::eMax];
	OmnMutexPtr		mLock;

public:
	AosErrorMgr();
	~AosErrorMgr();

	// Singleton Class Interface
	static AosErrorMgr*    getSelf();
	virtual bool	start();
	virtual bool 	stop();
	virtual bool	config(const AosXmlTagPtr &def);

	OmnString getErrmsg(const AosLocale::E locale, const AosErrmsgId::E error_id);
	OmnString getDirname() const {return mDirname;}

private:
	AosErrMsgMgrPtr getErrorMsgMgr(const AosLocale::E locale);
};
#endif

