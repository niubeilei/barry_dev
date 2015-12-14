////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliLevel.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelInterface_CliLevel_h
#define aos_KernelInterface_CliLevel_h

class AosCliLvl
{
public:
	enum CliLevel 
	{
		eUser			=	1,
		eEnable			=	2,
		eConfig 		=	3,
		eEngineering	=	4
	};


};
#endif
