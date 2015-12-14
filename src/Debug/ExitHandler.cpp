////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ExitHandler.cpp
// Description:
//	If program encounters a fatal error and it has to exit, it should call
//  handleExit() member function. Never call exit() directly because
//  we want to control when and how to exit.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Debug/ExitHandler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"



OmnExitHandler::OmnExitHandler()
{
}


OmnExitHandler::~OmnExitHandler()
{
}


int		
OmnExitHandler::handleExit(const OmnString &fileName, 
						   const int lineNo,
						   const OmnErrId::E exitId,
						   const OmnString &errMsg)
{
	OmnTrace << "Program needs to exit at: ["
		<< fileName << ":" << lineNo
		<< ". Exit ID = " << exitId
		<< ". ErrMsg = " << errMsg << endl;
	exit(-1);
	return -1;
}


void
OmnExitHandler::exitApp(
		const OmnString &fileName, 
		const int lineNo,
		const OmnString &errMsg)
{
	OmnAlarm << "Program needs to exit at: ["
		<< fileName << ":" << lineNo
		<< ". ErrMsg = " << errMsg << enderr;
	exit(-1);
}

