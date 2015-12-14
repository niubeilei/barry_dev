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
// 09/18/2010	Created by James 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_WordParser_WordParserEngine_h
#define AOS_WordParser_WordParserEngine_h

#include "WordParser/ICTCLAS50.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/SPtr.h"

OmnDefineSingletonClass(AosWordParserEngineSingleton,
						AosWordParserEngine,
						AosWordParserEngineSelf,
						OmnSingletonObjId::eWordParserEngine,
						"WordParserEngine");

class AosWordParserEngine : virtual public OmnRCObject
{
	OmnDefineRCObject;
	OmnRwLockPtr mRwLock;

public:
	AosWordParserEngine();
	~AosWordParserEngine();
    
	// Singleton class interface
	static AosWordParserEngine*	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &config);

	LPICTCLAS_RESULT paragraphProcessA(const char* src, const int strlen, int &rsltlen);
	void resultFree(LPICTCLAS_RESULT rslt);
	void importUserDictFile(const OmnString &usrdic);
};

#endif
