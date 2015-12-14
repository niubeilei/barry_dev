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
#ifndef Aos_SEInterfaces_DataSenderObj_h
#define Aos_SEInterfaces_DataSenderObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataSenderType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataSenderObj : virtual public OmnRCObject
{
protected:
	OmnString				mName;
	AosDataSenderType::E	mType;
	bool					mIsTemplate;

public:
	AosDataSenderObj()
	:
	mType(AosDataSenderType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosDataSenderObj(
				const OmnString &name, 
				const AosDataSenderType::E type, 
				const bool reg);
	~AosDataSenderObj();

	// AosDataSender Interface
	virtual bool send(
				const AosBuffPtr &input, 
				const AosRundataPtr &rdata) = 0;

	virtual AosDataSenderObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosDataSenderObjPtr getDataSenderStatic(
				const AosDataSenderType::E type, 
				const AosRundataPtr &rdata);

	static AosDataSenderObjPtr getDataSenderStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerDataSender(const AosDataSenderObjPtr &partitioner);
};
#endif

