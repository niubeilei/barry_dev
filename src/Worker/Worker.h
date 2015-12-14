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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Worker_Worker_h
#define Aos_Worker_Worker_h

#include "Worker/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosWorker : public OmnRCObject
{
	OmnDefineRCObject;

protected:
	OmnString		mWorkerType;

public:
	AosWorker();

	AosWorker(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc);
	~AosWorker();

	virtual bool run(		const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);

	virtual bool config(	const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);

	virtual AosWorkerPtr clone() const;

	virtual bool serializeTo(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff) const;

	virtual bool serializeFrom(
							const AosRundataPtr &rdata, 
							const AosBuffPtr &buff);

	virtual OmnString toString() const;
};

#endif

