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
// 12/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MediaData_AllDataPicker_h
#define Aos_MediaData_AllDataPicker_h

class AosAllDataPickers
{
public:
	AosAllDataPickers();

	bool check() {return true;}
};
extern AosAllDataPickers gAosAllDataPickers;
#endif
