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
// 09/29/2012 Created by Chen Ding
// 2013/05/29 Renamed to Jimo by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoDummy_h
#define Aos_Jimo_JimoDummy_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosJimoDummy : virtual public AosJimo
{
	OmnDefineRCObject;

public:
	AosJimoDummy();
	~AosJimoDummy();

	virtual bool run(		const AosRundataPtr &rdata);
	virtual bool run(		const AosRundataPtr &rdata, 
							const AosXmlTagPtr &worker_doc);
	void *getMethod( 		const AosRundataPtr &rdata, 
							const OmnString &name, 
							AosJimoUtil::funcmap_t &map, 
							AosMethodId::E &method_id);
	virtual OmnString toString() const;
	virtual AosJimoPtr cloneJimo()  const;
};
#endif

