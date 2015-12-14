////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/03/21 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataletPicker_DataPickerObj_h
#define AOS_DataletPicker_DataPickerObj_h

#include "Capplet/Capplet.h"

class DataPickerObj : virtual public OmnRCObject
{
public:
	virtual bool procDatalet(
			const AosRundataPtr &rdata, 
			const AosDataletPtr &datalet, 
			const OmnString &query) = 0;

	virtual bool procDatalet(
			const AosRundataPtr &rdata, 
			const vector<AosDataletPtr> &datalets, 
			const OmnString &query) = 0;

	virtual bool dataProcFailed(
			const AosRundataPtr &rdata, 
			const OmnString &errmsg, 
			const OmnString &query) = 0;
};
#endif

