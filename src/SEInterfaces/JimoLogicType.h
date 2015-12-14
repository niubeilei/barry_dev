////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2013/12/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JimoLogicType_h
#define Aos_SEInterfaces_JimoLogicType_h


class AosJimoLogicType
{
public:
	enum E
	{
		eInvalid, 

		eDataset,
		eDataProc,
		eDataProcDoc,
		eDataProcIndex,
		eDataProcUnion,
		eDataProcSyncher,
		eDataProcTransTorturer,
		eDataProcOnNetOffNet,
		eMax
	};
};
#endif

