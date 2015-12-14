////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticUtil_MemAllocType_h
#define Aos_SemanticUtil_MemAllocType_h

#include <string>

enum AosMemAllocType
{
	eAosMAT_Stack, 
	eAosMAT_Heap,
	eAosMAT_LocalStatic,
	eAosMAT_GlobalStatic,
	eAosMAT_Global
};

extern std::string	AosMemAllocType_toStr(const AosMemAllocType code);
extern AosMemAllocType AosMemAllocType_toEnum(const std::string &name);

#endif
