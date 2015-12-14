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
//
// Modification History:
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEINterfaces_IILScannerListener_h
#define Aos_SEINterfaces_IILScannerListener_h

#include "IILUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"

class AosIILScannerListener : virtual public OmnRCObject
{
public:
	virtual bool dataRetrieved(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata) = 0;
	virtual bool dataRetrieveFailed(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata) = 0;
	virtual bool noMoreData(
					const AosIILScannerObjPtr &scanner, 
					const AosRundataPtr &rdata) = 0;
};
#endif

