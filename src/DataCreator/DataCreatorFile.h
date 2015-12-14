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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_SorterRecord_h
#define Aos_Sorter_SorterRecord_h

#include "Sorter/Sorter.h"


class AosSorterRecord : virtual public AosSorter
{
	OmnDefineRCObject;

private:

public:
	AosSorterRecord(const bool regflag);
	AosSorterRecord(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosSorterRecord();

	// AosSorter Interface
	virtual bool sort(
				const AosDataTablePtr &input, 
				const AosRundataPtr &rdata);

	virtual AosSorterObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata);

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

