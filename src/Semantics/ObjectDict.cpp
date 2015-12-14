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
#include "Semantics/ObjectDict.h"

#include "aosUtil/Alarm.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Semantics/ObjDesc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"

OmnSingletonImpl(AosObjectDictSingleton,
		 		 AosObjectDict,
		 		 AosObjectDictSelf,
		 		 "AosObjectDict");


AosObjectDict::AosObjectDict()
{
}


AosObjectDict::~AosObjectDict()
{
}


bool
AosObjectDict::start()
{
	return true;
}


bool
AosObjectDict::stop()
{
	return true;
}
 

OmnRslt
AosObjectDict::config(const OmnXmlParserPtr &def)
{
	return true;
}
 

// 
// This function retrieves the object descriptor for the
// object 'obj'. If the object is not defined in this
// dictionary, it returns 0.
//
AosObjDescPtr
AosObjectDict::getObjDesc(void *obj)
{
	AosObjDesc objdesc(obj);
	AosObjDescPtr tmpPtr(&objdesc, false);
	AosObjDescPtr theObj = mObjTable.get(tmpPtr);
	return theObj;
}

