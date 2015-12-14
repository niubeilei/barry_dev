////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonImpl.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

#include "AppMgr/App.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"

#ifndef OmnSingletonSgLock
#define OmnSingletonSgLock
static OmnMutex			sgSingletonClassObjLock;
#endif

#define OmnSingletonImpl(singletonclass, classname, classself, strname) \
																\
classname * classself = NULL;									\
																\
classname *														\
classname::getSelf()											\
{																\
	sgSingletonClassObjLock.lock();								\
	if (!classself)												\
	{															\
		classself = new classname();							\
		sgSingletonClassObjLock.unlock();						\
		/*singletonclass::startSingleton1(false);*/					\
		return classself;										\
	}															\
	sgSingletonClassObjLock.unlock();							\
	return classself;											\
}																\
																\
bool															\
singletonclass::createSingleton()								\
{																\
	classname::getSelf();										\
	return true;												\
}																\
																\
																\
bool															\
singletonclass::startSingleton1(const bool needToLock)			\
{																\
	if (needToLock)												\
	{															\
		sgSingletonClassObjLock.lock();							\
	}															\
																\
	OmnRslt rslt = classself->config(OmnApp::getAppConfig());	\
	if (!rslt)													\
	{															\
		if (needToLock)											\
		{														\
			sgSingletonClassObjLock.unlock();					\
		}														\
		return false;											\
	}															\
																\
	if (!classself->start())									\
	{															\
		if (needToLock)											\
		{														\
			sgSingletonClassObjLock.unlock();					\
		}														\
		OmnAlarm << "Failed to start singleton: "				\
			<< strname << enderr;								\
		return false;											\
	}															\
																\
	if (needToLock)												\
	{															\
		sgSingletonClassObjLock.unlock();						\
	}															\
	return true;												\
}																\
																\
bool															\
singletonclass::stopSingleton()									\
{																\
	if (!classself)												\
	{															\
		OmnAlarm << "To stop singleton: "						\
			<< strname											\
			<< " but instance not created yet"					\
			<< enderr;											\
		return false;											\
	}															\
																\
	if (!classself->stop())										\
	{															\
		OmnAlarm << "Failed to stop singleton: "				\
			<< strname << enderr;								\
		return false;											\
	}															\
																\
	return true;												\
}																\
																\
bool															\
singletonclass::config(const AosXmlTagPtr &def)				\
{																\
	return classself->config(def);								\
}
