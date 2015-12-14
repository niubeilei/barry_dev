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
#ifndef Aos_SEInterfaces_DataCreatorObj_h
#define Aos_SEInterfaces_DataCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataCreatorType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataCreatorObj : virtual public OmnRCObject
{
protected:
	OmnString				mName;
	AosDataCreatorType::E	mType;
	bool					mIsTemplate;

public:
	AosDataCreatorObj()
	:
	mType(AosDataCreatorType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosDataCreatorObj(
				const OmnString &name, 
				const AosDataCreatorType::E type, 
				const bool reg);
	~AosDataCreatorObj();

	// AosDataCreator Interface
	virtual bool sort(
				const AosDataTablePtr &input, 
				const AosRundataPtr &rdata) = 0;

	virtual AosDataCreatorObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosDataCreatorObjPtr getDataCreatorStatic(
				const AosDataCreatorType::E type, 
				const AosRundataPtr &rdata);

	static AosDataCreatorObjPtr getDataCreatorStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerDataCreator(const AosDataCreatorObjPtr &partitioner);
};
#endif

