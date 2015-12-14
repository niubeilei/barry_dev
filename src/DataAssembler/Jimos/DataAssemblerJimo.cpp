////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/Jimos/DataAssemblerJimo.h"




AosDataAssemblerJimo::AosDataAssemblerJimo(
		const OmnString &name,
		const OmnString &version)
:
AosJimo(AOS_JIMOTYPE_DATA_ASSEMBLER, version),
AosDataAssembler(AosDataAssemblerType::eJimoDataAssembler, name, "???", 0)
{
}


AosDataAssemblerJimo::~AosDataAssemblerJimo()
{
}


