////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ExceptHandler.h
// Description:
//	When any exception is raised, it logs with this class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_ExcptHandler_h
#define Omn_Debug_ExcptHandler_h


class OmnExcptHandler OmnDeriveFromObj
{
	//
	// Do not use the two.
	//
	OmnExcptHandler(const OmnExcptHandler &rhs);
	OmnExcptHandler & operator = (const OmnExcptHandler &rhs);

public:
	OmnExcptHandler();
	~OmnExcptHandler();

	static void	log(const OmnString &fileName, 
					const int lineNo,
					const OmnExcpt &e);
};
#endif
