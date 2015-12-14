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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Chen Ding, 05/24/2012
Please use SEInterfaces/DataProcStatus.h
#ifndef Aos_DataFormat_DataProcStatus_h
#define Aos_DataFormat_DataProcStatus_h


class AosDataProcStatus
{
public:
	enum E
	{
		eInvalid,

		eDataTooShort,
		eRecordFiltered,
		eContinue,
		eError
	};
};

#endif
#endif
