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
// 06/28/2012 Created by Brian
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_FullHandlerActions_h
#define Aos_DataBlob_FullHandlerActions_h

#include "DataBlob/DataBlobFullHandler.h"

class AosFullHandlerActions : public AosDataBlobFullHandler 
{
public:
	AosFullHandlerActions(const AosXmlTagPtr &def, const AosRundataPtr &rdata){};
	~AosFullHandlerActions(){};
	
	virtual bool dataBlobFull(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata)
	{
		return true;
	}
};
#endif
