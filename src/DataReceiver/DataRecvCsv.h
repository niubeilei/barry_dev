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
// 2013/08/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_DataReceiver_DataRecvCsv_h
#define Aos_DataReceiver_DataRecvCsv_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/DataReceiverObj.h"
#include "XmlUtil/Ptrs.h"

class AosDataRecvCsv : public AosDataReceiverObj
{
	OmnDefineRCObject;

private:

public:
	AosDataRecvCsv();
	AosDataRecvCsv(const OmnString &version);
	virtual ~AosDataRecvCsv();

	// AosJimo Interface
	virtual bool run(   const AosRundataPtr &rdata);
	virtual AosJimoPtr cloneJimo()  const;
	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
	virtual void * getMethod(const AosRundataPtr &rdata, 
						const OmnString &name, 
						AosMethodId::E &method_id);
};

#endif
#endif

