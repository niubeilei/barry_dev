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
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoExample_JimoExample_h
#define Aos_JimoExample_JimoExample_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/Jimo.h"
#include "Util/ValueRslt.h"

class AosJimoExample : public AosJimo
{
	OmnDefineRCObject;

private:
	int			mData1;
	OmnString	mData2;

public:
	AosJimoExample(const AosRundataPtr &rdata, 
					const OmnString &objid,
					const AosXmlTagPtr &sdoc);
	~AosJimoExample();

	// Jimo Interface
	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff) const;
	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);
	virtual AosJimoPtr clone(
							const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc) const;
	virtual bool run(		const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc);
	virtual OmnString toString() const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const OmnString &interface_objid) const;
	virtual bool supportInterface(
							const AosRundataPtr &rdata, 
							const int interface_id) const;

private:
	bool config( 		const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);
	bool runWithSmartdoc(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &sdoc);
};
#endif
