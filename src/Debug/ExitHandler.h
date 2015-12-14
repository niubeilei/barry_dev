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
//	If program encounters a fatal error and it has to exit, it should call
//  handleExit() member function. Never call exit() directly because
//  we want to control when and how to exit.  
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Debug_ExitHandler_h
#define Omn_Debug_ExitHandler_h


#include "Util/String.h"
#include "Debug/ErrId.h"

#define OmnExit OmnExitHandler::exitApp(__FILE__, __LINE__, "Program Exit")
#define OmnExitApp(errmsg) OmnExitHandler::exitApp(__FILE__, __LINE__, (errmsg))

class OmnExitHandler 
{
public:
	OmnExitHandler();
	~OmnExitHandler();

	static int handleExit(const OmnString &fileName, 
						const int lineNo,
						const OmnErrId::E exitId,
						const OmnString &errMsg = "");
	static void exitApp(const OmnString &fname, 
						const int line, 
						const OmnString &errmsg);
};
#endif
