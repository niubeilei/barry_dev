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
// 12/05/2007	Modified by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Debug_ErrorMgr_h
#define Omn_Debug_ErrorMgr_h

#include "Debug/Ptrs.h"
#include "Porting/ThreadDef.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashObj.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(AosErrorMgrSingleton,
		  				AosErrorMgr,
		  				AosErrorMgrSelf,
		  				OmnSingletonObjId::eErrorMgr,
						"ErrorMgr");

#ifndef AosRaiseError
#define AosRaiseError(errmsg) 								\
			AosErrorMgrSelf->addError(__FILE__, __LINE__, 	\
			errmsg, "", OmnGetCurrentThreadId())
#endif

#ifndef aos_raise_error
#define aos_raise_error(errmsg)								\
			AosErrorMgrSelf->addError(__FILE__, __LINE__, 	\
			errmsg, "", OmnGetCurrentThreadId())
#endif


class AosErrorMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnObjHash<AosErrorPtr, 0xfff>	mErrors;

public:
	AosErrorMgr();
	virtual ~AosErrorMgr();

	// Singleton Class interface
	static AosErrorMgr*	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual bool		config(const AosXmlTagPtr &configData);

	bool	addError(const std::string &fname,
			 		 const int lineno,
			 		 const std::string &errmsg, 
			 		 const std::string &threadname, 
			 		 const int thread_id);

private:
};
#endif

#endif
