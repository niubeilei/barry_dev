////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/19/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WordProc_WordFilter_h
#define AOS_WordProc_WordFilter_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"



OmnDefineSingletonClass(AosWordFilterSingleton,
						AosWordFilter,
						AosWordFilterSelf,
						OmnSingletonObjId::eLogMgr,
						"WordFilter");


class AosWordFilter : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	bool		mIsStopping;

public:
	AosWordFilter();
	~AosWordFilter();

    //
    // Singleton class interface
    //
    static AosWordFilter*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosWordFilter";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
    virtual OmnSingletonObjId::E  
			getSysObjId() const 
			{
				return OmnSingletonObjId::eLogMgr;
			}

	u64	filter(const char *word, const int len);
};
#endif

