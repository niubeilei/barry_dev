////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecuredShell.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SecuredShell_ReadConfig_h
#define Omn_SecuredShell_ReadConfig_h

#include "Util/String.h"



class AosReadConfig
{
private:
	OmnString 	mFileName;

public:
	AosReadConfig(const OmnString &filename);
    ~AosReadConfig();	

private:
	bool 	readConfig();
};

#endif
