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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RecordFieldInfo_h
#define Aos_SEInterfaces_RecordFieldInfo_h

#include "Util/String.h"
#include "alarm_c/alarm.h"

struct RecordFieldInfo
{
	bool	  mIsDataProcInput;
	bool	  mIsDataEngineInput;
	OmnString mRecordName;
	OmnString mFieldName; 
	
	int		  mRecordIdx;
	int		  mFieldIdx;
	
	RecordFieldInfo()
	:
	mIsDataProcInput(false),
	mIsDataEngineInput(false),
	mRecordIdx(-1),
	mFieldIdx(-1)
	{};
	
	bool init(const OmnString &name_field, bool is_data_proc_input)
	{
		int loc = name_field.find('.', false);
		aos_assert_r(loc > 0, false);
		mRecordName = name_field.substr(0, loc-1);	
		mFieldName = name_field.substr(loc+1);
		mIsDataProcInput = is_data_proc_input;
		return true;
	};
	
	bool init(const OmnString &name, const OmnString &field, bool is_data_proc_input)
	{
		mRecordName = name;
		mFieldName = field;
		mIsDataProcInput = is_data_proc_input;
		return true;
	};

};

#endif

