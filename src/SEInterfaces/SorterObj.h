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
#ifndef Aos_SEInterfaces_SorterObj_h
#define Aos_SEInterfaces_SorterObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/SorterType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosSorterObj : virtual public OmnRCObject
{
protected:
	OmnString			mName;
	AosSorterType::E	mType;
	bool				mIsTemplate;

public:
	AosSorterObj()
	:
	mType(AosSorterType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosSorterObj(
				const OmnString &name, 
				const AosSorterType::E type, 
				const bool reg);
	~AosSorterObj();

	// AosSorter Interface
	virtual bool sort(
				const AosDataTablePtr &input, 
				const AosRundataPtr &rdata) = 0;

	// Chen Ding, 10/30/2012
	virtual bool sort(const AosRundataPtr &rdata) = 0;

	virtual AosSorterObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosSorterObjPtr getSorterStatic(
				const AosSorterType::E type, 
				const AosRundataPtr &rdata);

	static AosSorterObjPtr getSorterStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerSorter(const AosSorterObjPtr &partitioner);
};
#endif

