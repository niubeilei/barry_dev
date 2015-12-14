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
// Modification History:
// 2015/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UtilData_QueryProcStage_h
#define Aos_UtilData_QueryProcStage_h

struct AosQueryProcStage
{
	enum E
	{
		eInvalid,

		eStage1_Parsed, 
		eStage2_ReadyProcess,
		eStage3_PostProcess,
		eStage4_Finishing,

		eMaxEntry
	};
};
#endif

