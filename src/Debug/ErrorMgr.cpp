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
// ErrorMgr is a singleton class. It collects all the errors that were
// generated during program execution. It keeps a list of AosError instances.
// When a new error is reported, it checks whether an instance of that 
// error has been generated. Errors are identified by (filename, lineno).
// This means that errors generated in the same location are considered
// equivalent. 
//
// Modification History:
// 12/05/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Debug/ErrorMgr.h"

#include "Debug/Error.h"
#include "Debug/Debug.h"
#include "alarm_c/alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosErrorMgrSingleton,
		 		 AosErrorMgr,
		 		 AosErrorMgrSelf,
		 		 "AosErrorMgr");


AosErrorMgr::AosErrorMgr()
{
}


AosErrorMgr::~AosErrorMgr()
{
}


bool
AosErrorMgr::start()
{
	return true;
}


bool
AosErrorMgr::stop()
{
	OmnTrace << "Singleton class AosErrorMgr stopped!" << endl;
	return true;
}
 

bool
AosErrorMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool	
AosErrorMgr::addError(const std::string &fname,
			 		 const int lineno,
			 		 const std::string &errmsg, 
			 		 const std::string &threadname, 
			 		 const int thread_id)
{
	// 
	// Check whether the same error has been created in the table.
	//
	AosError err(fname, lineno, errmsg, threadname, thread_id);
	AosErrorPtr errPtr(&err, false);
	AosErrorPtr ee = mErrors.get(errPtr);
	if (ee)
	{
		return ee->addNewInstance(errmsg);
	}

	return mErrors.add(errPtr);
}

#endif
