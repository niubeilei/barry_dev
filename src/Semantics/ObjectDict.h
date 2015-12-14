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
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_Semantics_ObjectDict_h
#define Aos_Semantics_ObjectDict_h

#include "Semantics/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashObj.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(AosObjectDictSingleton,
		  				AosObjectDict,
		  				AosObjectDictSelf,
		  				OmnSingletonObjId::eObjectDict,
						"ObjectDict");


class AosObjectDict : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnObjHash<AosObjDescPtr, 0x0fff>	mObjTable;

public:
	AosObjectDict();
	virtual ~AosObjectDict();

	// 
	// Singleton Class interface
	//
	static AosObjectDict*	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual OmnRslt		config(const OmnXmlParserPtr &configData);

	AosObjDescPtr	getObjDesc(void *);

private:
};

#endif

