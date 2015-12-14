////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/05/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataGen_DataForQuery_h
#define Aos_DataGen_DataForQuery_h

#include "Jimo/Jimo.h"

class AosDataForQuery : public AosJimo
{
	OmnDefineRCObject;

private:

public:
	AosDataForQuery(const int version);
	~AosDataForQuery();
	
	// Jimo interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool run(const AosRundataPtr &rdata);
	virtual bool config( const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);
};
#endif

