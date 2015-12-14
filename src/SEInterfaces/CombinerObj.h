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
#ifndef Aos_SEInterfaces_CombinerObj_h
#define Aos_SEInterfaces_CombinerObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/CombinerType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosCombinerObj : virtual public OmnRCObject
{
protected:
	OmnString			mName;
	AosCombinerType::E	mType;
	bool				mIsTemplate;

public:
	AosCombinerObj()
	:
	mType(AosCombinerType::eInvalid),
	mIsTemplate(false)
	{
	}

	AosCombinerObj(
				const OmnString &name, 
				const AosCombinerType::E type, 
				const bool reg);
	~AosCombinerObj();

	// AosCombiner Interface
	virtual bool combine(
				const AosDataTablePtr &input, 
				AosDataTablePtr &output, 
				const AosRundataPtr &rdata) = 0;

	virtual AosCombinerObjPtr clone(
				const AosXmlTagPtr &def, 
				const AosRundataPtr &rdata) = 0;

	static AosCombinerObjPtr getCombinerStatic(
				const AosCombinerType::E type, 
				const AosRundataPtr &rdata);

	static AosCombinerObjPtr getCombinerStatic(
				const AosXmlTagPtr &item, 
				const AosRundataPtr &rdata);

private:
	static bool registerCombiner(const AosCombinerObjPtr &combiner);
};
#endif

