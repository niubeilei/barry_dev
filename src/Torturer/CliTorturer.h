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
// 01/04/2008: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Torturer_CliTorturer_h
#define Aos_Torturer_CliTorturer_h

#include "Torturer/Torturer.h"

class AosCliTorturer : public AosTorturer
{
protected:

public:
	AosCliTorturer(const std::string& torturerName, const std::string& productName);
	virtual ~AosCliTorturer();

};

#endif

